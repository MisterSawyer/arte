#pragma once
#include <asio.hpp>
#include <cstdio>
#include <filesystem>
#include <format>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <transaction/transaction.h>
#include <type/type.h>

namespace arte::python {
	class CPyObject;

	class CPyObject {
	  public:
		CPyObject() : _obj(nullptr) {}

		CPyObject(PyObject* obj) : _obj(obj) {}

		CPyObject(CPyObject&& other) : _obj(other._obj)
		{
			other._obj = nullptr;
		}

		~CPyObject() { Release(); }

		PyObject* GetObject() { return _obj; }

		PyObject* SetObject(PyObject* obj) { return (_obj = obj); }

		PyObject* AddRef()
		{
			if (_obj) { Py_INCREF(_obj); }
			return _obj;
		}

		void Release()
		{
			if (_obj) { Py_DECREF(_obj); }

			_obj = nullptr;
		}

		PyObject* operator->() { return _obj; }

		bool Is() { return _obj ? true : false; }

		PyObject* operator*() { return _obj; }

		PyObject* operator=(PyObject* obj)
		{
			_obj = obj;
			return _obj;
		}

		operator bool() { return _obj ? true : false; }

		PyObject** Address() { return &_obj; }

	  private:
		PyObject* _obj;
	};

	class API {
	  public:
		static bool Init();
		static void Close();
		static bool LoadScript(std::filesystem::path path, int argc = 0,
							   wchar_t* argv[] = nullptr);

		// api calls

		static PyObject* RegisterType(PyObject* self, PyObject* args);

		static PyObject* RegisterTransaction(PyObject* self, PyObject* args);

		static PyObject* InitTransaction(PyObject* self, PyObject* args);

		static PyObject* RunTransaction(PyObject* self, PyObject* args);

		static bool OnCall(std::size_t n, CPyObject argList);

	  protected:
	  private:
		static std::atomic_flag _initialized;

		static std::unordered_map<type::Type::id_type, type::Type>
			_registeredTypes;
		static std::unordered_map<std::string, type::Type::id_type>
			_typeNameToId;

		static transaction::Registry _transactionRegistry;
	};

	constinit inline static struct PyMethodDef API_Methods[] = {
		{"RegisterType", API::RegisterType, METH_VARARGS, "Register new type"},
		{"RegisterTransaction", API::RegisterTransaction, METH_VARARGS,
		 "Register transaction"},
		{"InitTransaction", API::InitTransaction, METH_VARARGS,
		 "InitTransaction"},
		{"RunTransaction", API::RunTransaction, METH_VARARGS, "RunTransaction"},

		{NULL, NULL, 0, NULL}};

	template <class Arg, class... Args> struct ParseFormatStr {
		constexpr inline std::string operator()() const
		{
			return ParseFormatStr<Arg>{}() + ParseFormatStr<Args...>{}();
		}
	};

	template <> struct ParseFormatStr<int> {
		constexpr inline std::string operator()() const { return "i"; }
	};

	template <> struct ParseFormatStr<std::size_t> {
		constexpr inline std::string operator()() const { return "u"; }
	};

	template <> struct ParseFormatStr<char*, Py_ssize_t> {
		constexpr inline std::string operator()() const { return "s#"; }
	};

	template <class T> struct ParseFormatStr<std::optional<T>> {
		constexpr inline std::string operator()() const
		{
			return "|" + ParseFormatStr<T>{}();
		}
	};

	template <class T> struct ParseFormatStr<std::vector<T>> {
		constexpr inline std::string operator()() const { return "O"; }
	};

	template <class... Args> std::tuple<Args...> ParseArgs(PyObject* args)
	{
		std::string parseStr = ParseFormatStr<Args...>{}();

		std::tuple<Args...> retTuple;

		bool ok = std::apply(
			[&args, &parseStr](Args&... argsBuffer) -> bool {
				return PyArg_ParseTuple(args, parseStr.c_str(), &argsBuffer...);
			},
			retTuple);

		return retTuple;
	}
}  // namespace arte::python
