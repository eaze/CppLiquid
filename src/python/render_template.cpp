// This wraps CppLiquid to provide a Python module for rendering Liquid templates.
//
// Example usage:
//
//   In [1]: import pyliquid
//
//   In [2]: pyliquid.render_template('{{ a | plus: b }}', {'a': 1, 'b': 10})
//   Out[2]: '11'
#include <iostream>
#include <limits>
#include <string>

#ifdef __APPLE__
#include <Python/Python.h>
#else
#include <Python.h>
#endif

#include "../liquid/template.hpp"

bool long_to_int(long l, int& i) {
  long min_value = std::numeric_limits<int>::min();
  if (l < min_value) {
    PyErr_Format(PyExc_ValueError,
        "Numeric parameter %ld exceeds min value %ld", l, min_value);
    return false;
  }

  long max_value = std::numeric_limits<int>::max();
  if (l > max_value) {
    PyErr_Format(PyExc_ValueError,
        "Numeric parameter %ld exceeds max value %ld", l, max_value);
    return false;
  }

  i = static_cast<int>(l);
  return true;
}

// Return a rendered Liquid template using the provided template parameters. The first
// argument must be a template string. The second argument must be a dict<String, Object>
// containing the parameters. An empty dict must be provided if no parameters are used.
extern "C" PyObject* render_template(PyObject* module, PyObject* args) {
  char* templ_text;
  PyObject* dict;
  PyArg_ParseTuple(args, "sO!", &templ_text, &PyDict_Type, &dict);
  if (PyErr_Occurred()) return nullptr;

  Liquid::Data::Hash params;
  PyObject* key;
  PyObject* value;
  Py_ssize_t idx = 0;
  while (PyDict_Next(dict, &idx, &key, &value)) {
    if (!PyString_Check(key)) {
      PyErr_Format(PyExc_TypeError,
          "Invalid template parameter key type: %s", key->ob_type->tp_name);
      return nullptr;
    }
    char* param_name = PyString_AsString(key);
    if (value == Py_None) {
      params[param_name] = Liquid::Data();
    } else if (PyString_Check(value)) {
      params[param_name] = PyString_AsString(value);
    } else if (PyBool_Check(value)) {
      params[param_name] = value == Py_True;
    } else if (PyInt_Check(value)) {
      long l = PyInt_AsLong(value);
      int i;
      if (!long_to_int(l, i)) Py_RETURN_NONE;
      params[param_name] = i;
    } else if (PyLong_Check(value)) {
      long l = PyLong_AsLong(value);
      int i;
      if (!long_to_int(l, i)) Py_RETURN_NONE;
      params[param_name] = i;
    } else if (PyFloat_Check(value)) {
      params[param_name] = PyFloat_AsDouble(value);
    } else {
      PyErr_Format(PyExc_TypeError,
          "Invalid template parameter value type: %s", value->ob_type->tp_name);
      return nullptr;
    }
  }
  Liquid::Data context(params);

  Liquid::Template templ;
  try {
    templ.parse(templ_text);
    const std::string& output = templ.render(context).toStdString();
    return Py_BuildValue("s", output.c_str());
  } catch (const std::exception& e) {
      PyErr_Format(PyExc_Exception, "Unable to render template: %s", e.what());
      return nullptr;
  }
}

static PyMethodDef kPyLiquidMethods[] = {
  {"render_template", render_template, METH_VARARGS, "Render a Liquid template."},
  {NULL, NULL, 0, NULL}  /* Sentinel */
};

PyMODINIT_FUNC initpyliquid(void) {
  Py_InitModule("pyliquid", kPyLiquidMethods);
}
