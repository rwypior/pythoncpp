import sys
import os

def testthirdparty():
    print("Third-party module example")

    path = sys.path

    print(f"{path}")

    from pyglm import glm

    vec = glm.vec3(1, 0, 0)

    print("Some vector:")
    print(vec)
