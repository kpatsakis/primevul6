ALGgetattro(PyObject *s, PyObject *attr)
{
  ALGobject *self = (ALGobject*)s;

  if (!PyString_Check(attr))
	goto generic;

  if (PyString_CompareWithASCIIString(attr, "IV") == 0)
    {
      return(PyBytes_FromStringAndSize((char *) self->IV, BLOCK_SIZE));
    }
  if (PyString_CompareWithASCIIString(attr, "mode") == 0)
     {
       return(PyInt_FromLong((long)(self->mode)));
     }
  if (PyString_CompareWithASCIIString(attr, "block_size") == 0)
     {
       return PyInt_FromLong(BLOCK_SIZE);
     }
  if (PyString_CompareWithASCIIString(attr, "key_size") == 0)
     {
       return PyInt_FromLong(KEY_SIZE);
     }
  generic:
#if PYTHON_API_VERSION >= 1011          /* Python 2.2 and later */
	return PyObject_GenericGetAttr(s, attr);
#else
	if (PyString_Check(attr) < 0) {
		PyErr_SetObject(PyExc_AttributeError, attr);
		return NULL;
	}
	return Py_FindMethod(ALGmethods, (PyObject *)self, PyString_AsString(attr));
#endif
}