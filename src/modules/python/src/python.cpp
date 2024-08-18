#include <python/python.h>

namespace arte::python {
	std::atomic_flag API::_initialized = ATOMIC_FLAG_INIT;
	std::unordered_map<type::Type::id_type, type::Type> API::_registeredTypes =
		{};
	std::unordered_map<std::string, type::Type::id_type> API::_typeNameToId =
		{};

	transaction::Registry API::_transactionRegistry = {};

	constinit inline static struct PyModuleDef modDef = {PyModuleDef_HEAD_INIT,
														 "arte",
														 NULL,
														 -1,
														 API_Methods,
														 NULL,
														 NULL,
														 NULL,
														 NULL};

	PyMODINIT_FUNC PyInit_arte(void) { return PyModule_Create(&modDef); }

	bool LoadModule()
	{
		if (PyImport_AppendInittab("arte", PyInit_arte) == -1) {
			std::puts("Error: could not extend in-built modules table\n");
			return false;
		}
		return true;
	}

	bool API::Init()
	{
		if (_initialized.test()) return true;

		PyConfig config;
		PyConfig_InitPythonConfig(&config);

		if (!LoadModule()) { return false; }

		PyStatus status = PyConfig_Read(&config);
		if (PyStatus_Exception(status)) {
			PyConfig_Clear(&config);
			return false;
		}

		status = Py_InitializeFromConfig(&config);
		if (PyStatus_Exception(status)) {
			PyConfig_Clear(&config);
			return false;
		}

		PyConfig_Clear(&config);
		_initialized.test_and_set();
		return true;
	}

	void API::Close()
	{
		_initialized.clear();
		Py_Finalize();
	}

	bool API::LoadScript(std::filesystem::path path, int argc, wchar_t* argv[])
	{
		if (!_initialized.test()) return false;
		if (argc > 0) PySys_SetArgv(argc, argv);

		CPyObject obj  = Py_BuildValue("s", path.string().c_str());
		FILE*	  file = _Py_fopen_obj(*obj, "r+");
		if (file != nullptr) {
			PyRun_SimpleFile(file, path.string().c_str());
			fclose(file);
		}
		else {
			std::puts("arte::python Failed to run script\n");
			return false;
		}

		return true;
	}

	bool API::OnCall(std::size_t n, CPyObject argList)
	{
		PyObject* pSysPath	  = PySys_GetObject("path");
		CPyObject pModulePath = PyUnicode_FromString(
			std::filesystem::current_path().string().c_str());
		PyList_Append(pSysPath, *pModulePath);

		CPyObject mainModule = PyImport_AddModule("__main__");
		if (!mainModule) {
			PyErr_Print();
			std::puts("arte::python Failed to access __main__ module\n");
			return false;
		}

		CPyObject mainDict = PyModule_GetDict(*mainModule);
		if (!mainDict) {
			PyErr_Print();
			std::puts(
				"arte::python Failed to access dictionary object from __main__ module\n");
			return false;
		}

		PyObject* OnCallFunc = PyDict_GetItemString(*mainDict, "OnCall");
		if (OnCallFunc && PyCallable_Check(OnCallFunc)) {
			CPyObject pN = PyLong_FromSize_t(n);
			// CPyObject argList = PyList_New(1);
			// PyList_SetItem(*argList, 0, Py_BuildValue("d", 2.5));

			CPyObject result =
				PyObject_CallFunctionObjArgs(OnCallFunc, *pN, *argList, NULL);
			if (result) {
				std::puts("called OnCallFunc()");
				if (PyList_Check(*result)) {
					std::puts("returned list of results");
				}
			}
			else {
				PyErr_Print();
				std::puts("Failed to call OnCallFunc()");
				return false;
			}
			Py_XDECREF(OnCallFunc);
		}

		return true;
	}

	PyObject* API::RegisterType(PyObject* self, PyObject* args)
	{
		Py_ssize_t	id;
		const char* name;
		Py_ssize_t	nameLen;
		bool		ok = PyArg_ParseTuple(args, "ns#", &id, &name, &nameLen);
		if (!ok) return PyLong_FromLong(1);

		type::Type t{.id = (type::Type::id_type)id, .name = name};

		std::puts(std::format("arte::python registering type `{}`", t).c_str());

		_registeredTypes.try_emplace(id, t);
		_typeNameToId.try_emplace(name, id);

		return PyLong_FromLong(0);
	}

	PyObject* API::RegisterTransaction(PyObject* self, PyObject* args)
	{
		Py_ssize_t	id;
		const char* name;
		Py_ssize_t	nameLen;
		PyObject*	insList;
		const char* retName;
		Py_ssize_t	retNameLen;
		// ID, NAME, IN, OUT
		if (!PyArg_ParseTuple(args, "ns#O!s#", &id, &name, &nameLen,
							  &PyList_Type, &insList, &retName, &retNameLen))
			return PyLong_FromLong(1);

		const char*						 inName;
		Py_ssize_t						 inNameLen;
		const char*						 inTypeName;
		Py_ssize_t						 inTypeNameLen;
		std::vector<type::Type::id_type> insVec;
		insVec.reserve(PyList_Size(insList));
		//[string, ...]
		for (unsigned currentArg = 0; currentArg < PyList_Size(insList);
			 ++currentArg) {
			PyObject* pItem = PyList_GetItem(insList, currentArg);
			if (!PyUnicode_Check(pItem)) {
				PyErr_SetString(PyExc_TypeError,
								"list items must be unicode strings");
				return PyLong_FromLong(1);
			}
			CPyObject		  s = PyUnicode_AsUTF8String(pItem);
			const std::string typeName(PyBytes_AsString(*s));
			if (!_typeNameToId.contains(typeName)) {
				std::puts(std::format("arte::python type `{}` not registered",
									  typeName)
							  .c_str());
				return PyLong_FromLong(1);
			}

			insVec.emplace_back(_typeNameToId.at(typeName));
		}

		if (!_typeNameToId.contains(retName)) {
			std::puts(
				std::format("arte::python type `{}` not registered", retName)
					.c_str());
			return PyLong_FromLong(1);
		}

		_transactionRegistry.Add(id, name, insVec, _typeNameToId.at(retName));

		std::puts(
			std::format(
				"arte::python registered transaction `{}` with return type {}",
				name, retName)
				.c_str());

		return PyLong_FromLong(0);
	}

	PyObject* API::InitTransaction(PyObject* self, PyObject* args)
	{
		Py_ssize_t	id;
		const char* name;
		Py_ssize_t	nameLen;
		PyObject*	resObj;

		if (!PyArg_ParseTuple(args, "ns#|O", &id, &name, &nameLen, &resObj))
			return PyLong_FromLong(1);

		return PyLong_FromLong(0);
	}

	PyObject* API::RunTransaction(PyObject* self, PyObject* args)
	{
		Py_ssize_t	id;
		Py_ssize_t	n;
		const char* name;
		Py_ssize_t	nameLen;
		PyObject*	argsObj;
		// arte.RunTransaction(ID, "gen_major_scale", n, random_samples)
		if (!PyArg_ParseTuple(args, "ns#n|O", &id, &name, &nameLen, &n,
							  &argsObj)) {
			Py_DECREF(argsObj);
			return PyLong_FromLong(1);
		}
		if (!PyList_Check(argsObj)) {
			std::puts("arte::python args must be passed as list");
			return PyLong_FromLong(1);
		}

		auto transactionRes = _transactionRegistry.Find(name);
		if (!transactionRes.has_value()) {
			Py_DECREF(argsObj);
			return PyLong_FromLong(1);
		}
		const auto& transaction = transactionRes.value();

		if (PyList_Size(argsObj) != transaction.get().ins.size()) {
			std::puts(
				std::format(
					"arte::python wrong number of arguments {} for function {}, expected `{}`",
					PyList_Size(argsObj), name, transaction.get().ins.size())
					.c_str());
			Py_DECREF(argsObj);
			return PyLong_FromLong(1);
		}

		std::puts(
			std::format("arte::python run transaction `{}`", name).c_str());

		CPyObject pName = PyUnicode_FromString(
			(std::string("scripts.transactions.") + name).c_str());
		if (!pName) {
			std::puts(
				std::format("arte::python error constructing module name `{}`",
							name)
					.c_str());
			return PyLong_FromLong(1);
		}
		PyObject* pModule = PyImport_Import(*pName);
		if (!pModule) {
			std::puts(
				std::format("arte::python error importing module `{}`", name)
					.c_str());
			return PyLong_FromLong(1);
		}

		PyObject* pFunc = PyObject_GetAttrString(pModule, "OnCall");
		if (!pFunc || !PyCallable_Check(pFunc)) {
			if (PyErr_Occurred()) PyErr_Print();
			std::puts("arte::python Cannot find function 'OnCall'");
			Py_XDECREF(pFunc);
			return PyLong_FromLong(1);
		}

		PyObject* pValue = PyObject_CallFunctionObjArgs(
			pFunc, PyLong_FromSize_t(n), argsObj, NULL);

		// Py_DECREF(argsObj);
		if (PyErr_Occurred()) { PyErr_Print(); }
		if (!pValue) {
			std::puts("arte::python Cannot retrive OnCall return value");
			Py_DECREF(pValue);
			Py_XDECREF(pFunc);
			return PyLong_FromLong(1);
		}

		if (!PyList_Check(pValue)) {
			std::puts("arte::python return values must be passed as list");
			Py_DECREF(pValue);
			Py_XDECREF(pFunc);
			return PyLong_FromLong(1);
		}
		/*
		for (Py_ssize_t i = 0; i < PyList_Size(pValue); ++i) {
			CPyObject pRow = PyList_GetItem(pValue, i);
			if(PyList_Check(*pRow))
			{
				std::puts("arte::python return values lists of lists");

			}
		}
		*/
		Py_XDECREF(pFunc);
		const auto& returnType =
			_registeredTypes.at(transaction.get().retT).name;
		return PyTuple_Pack(2, PyUnicode_FromString(returnType.c_str()),
							pValue);
	}
}  // namespace arte::python