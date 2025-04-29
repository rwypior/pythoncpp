import sys
import os
import sysconfig
import argparse
import shutil
import site
import filecmp
import subprocess
import re
import glob
import io
from contextlib import redirect_stdout
from pip._internal import main as pipmain
from functools import cmp_to_key
from pathlib import Path

class Unbuffered(object):
   def __init__(self, stream):
       self.stream = stream

   def write(self, data):
       self.stream.write(data)
       self.stream.flush()

   def writelines(self, datas):
       self.stream.writelines(datas)
       self.stream.flush()

   def __getattr__(self, attr):
       return getattr(self.stream, attr)

unbuffered_stdout = Unbuffered(sys.stdout)
sys.stdout = unbuffered_stdout

parser = argparse.ArgumentParser(
    prog="Python installer"
)
parser.add_argument("path", help="Where to install Python")
parser.add_argument("-a", "--auxpath", help="Where to install auxiliary components")
parser.add_argument("-d", "--dryrun", help="Only print commands without executing them", action="store_true")
parser.add_argument("-s", "--nositepackages", help="Skip site_packages directory", action="store_true")
parser.add_argument("-H", "--generateheader", help="Path to generated C header file specifying copied python paths")
parser.add_argument("-p", "--package", nargs="*", help="Download specified package in format name==version or only name")
parser.add_argument("-i", "--install", help="Install downloaded packages", action="store_true")
parser.add_argument("--wheeldir", help="Where to store downloaded wheel files")
parser.add_argument("--installdir", help="Where to install packages")
args = parser.parse_args()

print("Installing Python")

dryrun = args.dryrun
nosite = args.nositepackages
target = args.path
auxtarget = args.auxpath
generateheaderpath = args.generateheader
packages = args.package
installpackages = args.install
wheeldir = args.wheeldir
installdir = args.installdir

if not auxtarget:
    auxtarget = ""
auxtarget = os.path.join(target, auxtarget)

generated_paths = set()

# Utils

def copydifferent(src, dst):
    if not os.path.exists(dst) or not filecmp.cmp(src, dst):
        shutil.copy2(src, dst)

def printdry(*msg, onlydryrun=False):
    prefix = ""
    if dryrun:
        prefix = "[DRY RUN]: "
        
    if not onlydryrun or dryrun:
        m = " ".join(msg)
        print(f"{prefix}{m}")

def cmplength(a, b):
    if len(a) < len(b):
        return -1
    elif len(a) > len(b):
        return 1
    return 0

def isSubdirOf(subdir, parent):
    return Path(parent) in Path(subdir).parents

def isParentOf(parent, subdir):
    return os.path.normpath(parent) == os.path.normpath(Path(subdir).parent)

def excludeParents(paths, path):
    toRemove = None
    for parent in paths:
        if isParentOf(parent[0], path):
            toRemove = parent
            break
    
    if toRemove is not None:
        paths.remove(toRemove)

# Shared library

def findTargetPythonDll():
    pythondir = os.path.dirname(os.path.realpath(sys.executable)).lower()
    s = os.path.join(pythondir, "python*.dll")
    dlls = glob.glob(s)
    if len(dlls) >= 1:
        res = []
        for dll in dlls:
            res.append([dll, os.path.basename(dll)])
        return res
    return None
	
def findTargetPythonSo():
    soinfo = sysconfig.get_config_vars('LIBDIR', 'INSTSONAME')
    sopath = os.path.join(str(soinfo[0]), str(soinfo[1]))
	    
    found = bool(soinfo[0]) and bool(soinfo[1])
    if not found:
        return None
    
    sopath = os.path.join(str(soinfo[0]), str(soinfo[1]))
    return [[sopath, str(soinfo[1])]]

def copyLibrary():
    so = findTargetPythonSo()
    if so is None:
        so = findTargetPythonDll()
    if so is None:
        print("Shared library not found, skipping")

    for entry in so:
        sopath = entry[0]
        targetpath = os.path.join(target, entry[1])
        printdry(f"Copying \"{sopath}\" to \"{targetpath}\"")
        if not dryrun:
            shutil.copyfile(sopath, targetpath)

# Python modules

def copyAux():
    global generated_paths

    pythonpath = os.environ['PYTHONPATH'].split(os.pathsep) if 'PYTHONPATH' in os.environ else []
    path = [item for item in sys.path if item not in pythonpath]
    scriptdir = os.path.dirname(os.path.realpath(__file__)).lower()
    cdir = os.getcwd()
    pythondir = os.path.dirname(os.path.realpath(sys.executable)).lower()
    sitedir = sysconfig.get_path("platlib").lower()
    sitedirname = os.path.basename(os.path.normpath(sitedir))

    exclude = {"__pycache__", 'Doc', 'include', 'libs', 'tcl', 'Tools'}
    if nosite:
        sites = site.getsitepackages()
        for s in sites:
            leaf = os.path.basename(os.path.normpath(s))
            exclude.add(leaf)
    
    paths = set()
    additional_paths = set()

    for p in sorted(path, key=cmp_to_key(cmplength)):
        pl = p.lower()
        leaf = os.path.basename(os.path.normpath(pl))
        if pl == cdir or pl == scriptdir:
            continue
        if not os.path.exists(p):
            continue
        if os.path.isfile(p):
            continue
        if nosite and leaf == sitedirname:
            continue
 
        dirname = os.path.basename(os.path.normpath(p))
        copytarget = os.path.join(auxtarget, dirname)
        
        isSubdir = False
        for (path, target) in paths:
            if isSubdirOf(p, path):
                additional = os.path.join(auxtarget, os.path.basename(Path(p).parent), dirname)
                additional_paths.add((p, additional))
                isSubdir = True
                break
        if isSubdir:
            continue

        paths.add((p, copytarget))

    for p in paths:
        source = p[0]
        target = p[1]
        printdry(f"Copying \"{source}\" to \"{target}\" excluding {exclude}")
        if not dryrun:
            shutil.copytree(source, target, ignore=shutil.ignore_patterns(*exclude), copy_function=copydifferent, dirs_exist_ok=True)

    generated_paths = paths.union(additional_paths)
 
def findTargetPythonDirectory():
    s = os.path.join(auxtarget, "python*")
    res = glob.glob(s)
    if len(res) >= 1:
        return res[0]
		
    return auxtarget
		
# Wheels

def downloadWheel(package, version=None, dir=None):
    name = package
    if version is not None:
        name = f"{name}=={version}"

    params = ["wheel", name]
    if dir is not None:
        params.append("-w")
        params.append(dir)

    drydir = "" if dir is None else f"-w {dir}"
    printdry("Running pip ", " ".join(params))
    if not dryrun:
        result = io.StringIO()
        with redirect_stdout(result):
            try:
                pipmain(params)
            except:
                pass
        out = result.getvalue()
        
        found = re.findall("Saved (.+)$", out, re.M)
        if len(found) == 0:
            found = re.findall("File was already downloaded (.+)$", out, re.M)
            if len(found) == 0:
                return None
        return found[0].strip()

def installWheel(wheelpath, installpath):

    params = ["install", str(wheelpath), "--target", str(installpath)]
    printdry("Running pip ", " ".join(params))

    if not dryrun:
        result = io.StringIO()
        with redirect_stdout(result):
            try:
                pipmain(params)
            except:
                pass
        return result.getvalue()

def installPackages(pythonDir):
    targetWheelDir = installdir
    if targetWheelDir is None:
        if pythonDir is None:
            print("Python base must be installed before installing packages")
            return
        targetWheelDir = os.path.join(pythonDir, "site-packages")
    toInstall = []
    for package in packages:
        split = package.split("==")
        name = split[0]
        version = None
        if len(split) > 1:
            version = split[1]

        printdry(f"Downloading {name}")
        path = downloadWheel(name, version, wheeldir)
        if path is None:
            print(f"Skipping {package}")
            continue
        toInstall.append(path)

    if installpackages:
        generated_paths.add(("", targetWheelDir))
        for path in toInstall:
            installWheel(path, targetWheelDir)
			
# Definitions header

def generateHeader(generated_pythonhome):
    rel_generated_pythonhome = os.path.relpath(generated_pythonhome, target).replace('\\', '\\\\')

    hdr = ""
    hdr += "// This file is automatically generated by pythoncpp library scripts\n"
    hdr += "// and contains paths to be specified in C++ Python application.\n"
    hdr += "// Please do not modify contents of this file\n"
    hdr += "\n"
    hdr += "#include <string>\n"
    hdr += "#include <vector>\n"
    hdr += "\n"
    hdr += "namespace Python\n{\n"
    hdr += f"constexpr char pythonhome[] = \"{rel_generated_pythonhome}\";\n"
    hdr += "const std::vector<std::string> pythondirs{\n"
    for genpath in sorted(list(generated_paths), key=lambda p: p[1]):
        rel = os.path.relpath(genpath[1], target).replace('\\', '\\\\')
        hdr += f"\t\"{rel}\",\n"
    hdr = hdr[:-2]
    hdr += "\n};"
    hdr += "\n}"

    printdry(f"Writing python definitions header to {generateheaderpath}")
    if not dryrun:
        with open(generateheaderpath, 'w') as f:
            f.write(hdr)
        
# Entry point

copyLibrary()
copyAux()
pythonDir = findTargetPythonDirectory()
installPackages(pythonDir)
generateHeader(pythonDir)

print("Installing Python finished")

