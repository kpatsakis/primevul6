bytes_getnewargs(PyBytesObject *v)
{
    return Py_BuildValue("(y#)", v->ob_sval, Py_SIZE(v));
}