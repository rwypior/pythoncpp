#include "PythonCppCommon.hpp"
#include "PythonModule.h"
#include "PythonFunction.h"
#include "PythonDict.h"
#include "PythonClass.h"
#include "PythonException.h"

#include <sstream>
#include <vector>
#include <list>

namespace Python
{
	std::unordered_map<FunctionFlag, int> functionFlagMap {
		{ FunctionFlag::ClassMethod, METH_CLASS },
		{ FunctionFlag::Coexist, METH_COEXIST },
		{ FunctionFlag::Fastcall, METH_FASTCALL },
		{ FunctionFlag::Keywords, METH_KEYWORDS },
		{ FunctionFlag::NoArgs, METH_NOARGS },
		{ FunctionFlag::OneArg, METH_O },
		{ FunctionFlag::StaticMethod, METH_STATIC },
		{ FunctionFlag::VarArgs, METH_VARARGS }
	};

	int translateFunctionFlag(FunctionFlag flag)
	{
		int res = 0x00;
		for (auto pair : functionFlagMap)
		{
			if (!!(flag & pair.first))
			{
				res |= pair.second;
			}
		}
		return res;
	}

	struct PythonFunctionDefContainerImpl : public PythonFunctionDefContainer
	{
		std::vector<PyMethodDef> defs;
	};

	struct PythonClassDefContainerImpl : public PythonClassDefContainer
	{
		struct Definition
		{
			PyTypeObject object;
			std::vector<PyMethodDef> methods;
			std::vector<PyMemberDef> members;
		};
		std::list<Definition> defs;

		Definition& create() { return defs.emplace_back(); }
	};

	// Data setter

	class ModuleSetter : public Setter
	{
	public:
		explicit ModuleSetter(Module& module, const std::string& varName)
			: module(module)
			, varName(varName)
		{ }

		virtual void set(const Object& data) override
		{
			this->module.setVariable(this->varName, data);
		}

	private:
		Module& module;
		std::string varName;
	};

	// Synchronizer

	class ModuleSynchronizer : public Synchronizer
	{
	public:
		explicit ModuleSynchronizer(Module& module, const std::string& varName)
			: module(module)
			, varName(varName)
		{ }

		virtual void synchronize(pyobj& data) override
		{
			pyobj newobj = PyObject_GetAttrString(PO(this->module), this->varName.c_str());
			if (newobj != data)
			{
				Py_DECREF(data);
				data = newobj;
			}
		}

	private:
		Module& module;
		std::string varName;
	};

	// Module

	Module::Module(pyobj module, std::shared_ptr<Dict> globals)
		: module(module)
		, locals(module ? std::make_shared<Dict>(PyModule_GetDict(PO(module))) : nullptr)
		, globals(globals)
		, functions(std::make_unique<PythonFunctionDefContainerImpl>())
		, classes(std::make_unique<PythonClassDefContainerImpl>())
	{
		if (this->module)
			Py_INCREF(this->module);
	}

	Module::Module(const std::string& moduleName)
		: module(PyModule_New(moduleName.c_str()))
		, locals(std::make_shared<Dict>(PyModule_GetDict(PO(module))))
		, globals(std::make_shared<Dict>())
		, functions(std::make_unique<PythonFunctionDefContainerImpl>())
		, classes(std::make_unique<PythonClassDefContainerImpl>())
	{
	}

	Module::Module(Module&& module)
		: module(std::move(module.module))
		, locals(std::move(module.locals))
		, globals(std::move(module.globals))
		, functions(std::move(module.functions))
		, classes(std::move(module.classes))
	{
		module.module = nullptr;
	}

	Module::~Module()
	{
		if (this->module)
		{
			assert(Py_REFCNT(this->module) > 0);
			Py_DECREF(this->module);
		}
	}

	Module& Module::operator=(Module&& b)
	{
		this->module = std::move(b.module);
		this->locals = std::move(b.locals);
		this->globals = std::move(b.globals);
		this->functions = std::move(b.functions);
		this->classes = std::move(b.classes);
		b.module = nullptr;
		return *this;
	}

	Module::operator pyobj() const
	{
		if (this->module)
			Py_INCREF(this->module);
		return this->module;
	}

	void Module::reload()
	{
		this->module = PyImport_ReloadModule(PO(this->module));
	}

	Function Module::createFunction(const std::string& name, const std::string& source)
	{
		PyRun_String(source.c_str(), Py_file_input, PO(this->globals->data), PO(this->locals->data));
		checkError();

		return this->getFunction(name);
	}

	Function Module::getFunction(const std::string& name)
	{
		PyObject* function = PyObject_GetAttrString(PO(this->module), name.c_str());
		checkError();

		return Function(function);
	}

	void Module::addFunctions(const std::vector<Types::FunctionDefinition>& definitions)
	{
		PythonFunctionDefContainerImpl& defCnt = static_cast<PythonFunctionDefContainerImpl&>(*this->functions);

		for (const Types::FunctionDefinition& def : definitions)
		{
			defCnt.defs.push_back({ def.name.c_str(), reinterpret_cast<PyCFunction>(def.function), METH_VARARGS, def.docString.c_str() });
		}

		defCnt.defs.push_back({ NULL, NULL, NULL, NULL });

		PyModule_AddFunctions(PO(this->module), &defCnt.defs[0]);
		checkError();
	}

	Class Module::getClass(const std::string& name)
	{
		PyObject* cls = PyObject_GetAttrString(PO(this->module), name.c_str());
		checkError();

		return Class(cls);
	}

	Object Module::getVariable(const std::string& name)
	{
		PyObject* var = PyObject_GetAttrString(PO(this->module), name.c_str());
		checkError();
		return Object(var, new ModuleSetter(*this, name), new ModuleSynchronizer(*this, name));
	}

	void Module::setVariable(const std::string& name, const Object &variable)
	{
		PyObject_SetAttrString(PO(this->module), name.c_str(), PO(variable));
		checkError();
	}

	struct CustomObject
	{
		PyObject_HEAD;
		PyObject* dict;
	};

	PyObject* customReduce(PyObject* self, PyObject* /* arg */)
	{
		PyObject* type = PyObject_Type(self);

		PyObject* tuple = PyTuple_New(2);
		PyTuple_SetItem(tuple, 0, type);

		PyObject* tupleArgs = PyTuple_New(0);
		PyTuple_SetItem(tuple, 1, tupleArgs);

		return tuple;
	}

	static int CustomSetattr(PyObject* self, PyObject* attr, PyObject* value)
	{
		if (PyObject_HasAttrString(self, "__setattr"))
		{
			std::string attrname = py_str(attr);
			PyObject* args = PyTuple_New(2);
			PyTuple_SetItem(args, 0, attr);
			PyTuple_SetItem(args, 1, value);

			PyObject* notifFunction = PyObject_GetAttrString(self, "__setattr");
			PyObject_CallObject(notifFunction, args);
			Py_DECREF(args);
		}

		return PyObject_GenericSetAttr(self, attr, value);
	}

	void Module::addClasses(const std::vector<Types::ClassDefinition>& definitions)
	{
		PythonClassDefContainerImpl& defCnt = static_cast<PythonClassDefContainerImpl&>(*this->classes);

		for (const Types::ClassDefinition& def : definitions)
		{
			assert(def.typeSize != static_cast<size_t>(-1));

			const char *fullName = toUtf8(this->getName() + "." + def.name);
			const char* desc = toUtf8(def.docString);

			PythonClassDefContainerImpl::Definition &objDef = defCnt.create();

			// Create functions

			for (const Types::FunctionDefinition& functiondef : def.functionDefinitions)
			{
				int flags = translateFunctionFlag(functiondef.flags);
				objDef.methods.push_back({ functiondef.name.c_str(), reinterpret_cast<PyCFunction>(functiondef.function), flags, functiondef.docString.c_str() });
			}
			if (def.setAttrFunction)
				objDef.methods.push_back({ "__setattr", reinterpret_cast<PyCFunction>(def.setAttrFunction), METH_VARARGS, NULL });
			objDef.methods.push_back({ "__reduce__", reinterpret_cast<PyCFunction>(customReduce), METH_VARARGS, NULL });
			objDef.methods.push_back({ NULL, NULL, NULL, NULL });

			// Create members

			for (const Types::MemberDefinition& memberdef : def.memberDefinitions)
			{
				objDef.members.push_back({ 
					memberdef.name.c_str(), 
					translateType(memberdef.type),
					memberdef.offset + static_cast<__int64>(sizeof(CustomObject)),
					memberdef.readonly ? READONLY : 0, 
					memberdef.docstring.c_str() 
				});
			}

			objDef.members.push_back({
					"__dict__",
					translateType(Types::MemberDefinition::Type::Object),
					offsetof(CustomObject, dict),
					0,
					NULL
				});

			objDef.members.push_back({ NULL });

			// Functions

			newfunc newFunction = PyType_GenericNew;
			if (def.newFunction)
				newFunction = reinterpret_cast<newfunc>(def.newFunction);

			allocfunc allocFunction = PyType_GenericAlloc;
			if (def.allocFunction)
				allocFunction = reinterpret_cast<allocfunc>(def.allocFunction);

			// TODO - provide implementations for the rest of this stuff
			objDef.object = {
				/* base */					PyObject_HEAD_INIT(NULL)
				/* tp_name */				fullName,
				/* tp_basicsize */			static_cast<__int64>(sizeof(CustomObject) + def.typeSize),
				/* tp_itemsize */			0,
				/* tp_dealloc */			reinterpret_cast<destructor>(def.deallocFunction),
				/* tp_vectorcall_offset */	0,
				/* tp_getattr */			nullptr,
				/* tp_setattr */			0,
				/* tp_as_async */			0,
				/* tp_repr */				reinterpret_cast<reprfunc>(def.reprFunction),
				/* tp_as_number */			0,
				/* tp_as_sequence */		0,
				/* tp_as_mapping */			0,
				/* tp_hash */				nullptr,
				/* tp_call */				0,
				/* tp_str */				nullptr,
				/* tp_getattro */			PyObject_GenericGetAttr,
				/* tp_setattro */			reinterpret_cast<setattrofunc>(CustomSetattr),
				/* tp_as_buffer */			0,
				/* tp_flags */				Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
				/* tp_doc */				desc,
				/* tp_traverse */			0,
				/* tp_clear */				0,
				/* tp_richcompare */		0,
				/* tp_weaklistoffset */		0,
				/* tp_iter */				0,
				/* tp_iternext */			0,
				/* tp_methods */			&objDef.methods[0],
				/* tp_members */			objDef.members.empty() ? nullptr : &objDef.members[0],
				/* tp_getset */				0,
				/* tp_base */				0,
				/* tp_dict */				0, // This is for type object itself
				/* tp_descr_get */			0,
				/* tp_descr_set */			0,
				/* tp_dictoffset */			offsetof(CustomObject, dict),
				/* tp_init */				reinterpret_cast<initproc>(def.initFunction),
				/* tp_alloc */				allocFunction,
				/* tp_new */				newFunction,
			};
			
			PyTypeObject* obj = &objDef.object;

			if (PyType_Ready(obj) < 0)
				throw PythonException(std::string("Type \"") + fullName + "\" could not be finalized");
			
			Py_INCREF(obj);

			int result = PyModule_AddObject(PO(this->module), def.name.c_str(), reinterpret_cast<PyObject*>(obj));
			checkError();

			if (result < 0)
			{
				Py_DECREF(obj);
				Py_DECREF(this->module);
			}
		}
	}

	void Module::pushConstant(const std::string& name, const std::string& value)
	{
		PyModule_AddStringConstant(PO(this->module), name.c_str(), value.c_str());
		checkError();
	}

	void Module::pushImport(const std::string& name, Module mod)
	{
		this->globals->append(name, mod);
	}

	void Module::run(const std::string& code)
	{
		PyRun_String(code.c_str(), Py_single_input, PO(this->globals->data), PO(this->locals->data));
		checkError();
	}

	std::string Module::getName() const
	{
		const char* name = PyModule_GetName(PO(this->module));
		checkError();
		return std::string(name);
	}

	std::string Module::getDocstring() const
	{
		Object obj = Object(PyObject_GetAttrString(PO(this->module), "__doc__"));
		return obj.toString();
	}

	bool Module::exists(const std::string& name) const
	{
		bool ret = PyObject_HasAttrString(PO(this->module), name.c_str());
		checkError();
		return ret;
	}

	std::string Module::getPath() const
	{
		Object filename(PyModule_GetFilenameObject(PO(this->module)));
		checkError();
		return filename.toString();
	}

	// Utils

	void* getDataStructureImpl(void* self)
	{
		return reinterpret_cast<CustomObject*>(reinterpret_cast<unsigned char*>(self) + sizeof(CustomObject));
	}

	void* initializeClassImpl(void* type)
	{
		PyTypeObject* pyType = reinterpret_cast<PyTypeObject*>(type);
		void *obj = pyType->tp_alloc(pyType, 0);
		return getDataStructureImpl(obj);
	}

	void* baseObject(void* self)
	{
		return reinterpret_cast<CustomObject*>(reinterpret_cast<unsigned char*>(self) - sizeof(CustomObject));
	}
}