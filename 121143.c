ALGsetattr(PyObject *ptr, char *name, PyObject *v)
{
  ALGobject *self=(ALGobject *)ptr;
  if (strcmp(name, "IV") != 0) 
    {
      PyErr_Format(PyExc_AttributeError,
		   "non-existent block cipher object attribute '%s'",
		   name);
      return -1;
    }
  if (v==NULL)
    {
      PyErr_SetString(PyExc_AttributeError,
		      "Can't delete IV attribute of block cipher object");
      return -1;
    }
  if (!PyBytes_Check(v))
    {
      PyErr_SetString(PyExc_TypeError,
			  "IV attribute of block cipher object must be bytestring");
      return -1;
    }
  if (PyBytes_Size(v)!=BLOCK_SIZE) 
    {
      PyErr_Format(PyExc_ValueError, 
		   _MODULE_STRING " IV must be %i bytes long",
		   BLOCK_SIZE);
      return -1;
    }
  memcpy(self->IV, PyBytes_AsString(v), BLOCK_SIZE);
  return 0;
}