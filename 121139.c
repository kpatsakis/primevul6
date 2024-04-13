_MODULE_NAME (void)
{
	PyObject *m = NULL;
	PyObject *abiver = NULL;
	PyObject *__all__ = NULL;

	if (PyType_Ready(&ALGtype) < 0)
		goto errout;

	/* Create the module and add the functions */
#ifdef IS_PY3K
	m = PyModule_Create(&moduledef);
#else
	m = Py_InitModule("Crypto.Cipher." _MODULE_STRING, modulemethods);
#endif
	if (m == NULL)
		goto errout;

	/* Add the type object to the module (using the name of the module itself),
	 * so that its methods docstrings are discoverable by introspection tools. */
	PyObject_SetAttrString(m, _MODULE_STRING, (PyObject *)&ALGtype);

	/* Add some symbolic constants to the module */
	PyModule_AddIntConstant(m, "MODE_ECB", MODE_ECB);
	PyModule_AddIntConstant(m, "MODE_CBC", MODE_CBC);
	PyModule_AddIntConstant(m, "MODE_CFB", MODE_CFB);
	PyModule_AddIntConstant(m, "MODE_PGP", MODE_PGP); /** Vestigial **/
	PyModule_AddIntConstant(m, "MODE_OFB", MODE_OFB);
	PyModule_AddIntConstant(m, "MODE_CTR", MODE_CTR);
	PyModule_AddIntConstant(m, "block_size", BLOCK_SIZE);
	PyModule_AddIntConstant(m, "key_size", KEY_SIZE);

	/* Import CounterBE and CounterLE from the _counter module */
	Py_CLEAR(_counter_module);
	_counter_module = PyImport_ImportModule("Crypto.Util._counter");
	if (_counter_module == NULL)
		goto errout;
	PCT_CounterBEType = (PyTypeObject *)PyObject_GetAttrString(_counter_module, "CounterBE");
	PCT_CounterLEType = (PyTypeObject *)PyObject_GetAttrString(_counter_module, "CounterLE");

	/* Simple ABI version check in case the user doesn't re-compile all of
	 * the modules during an upgrade. */
	abiver = PyObject_GetAttrString(_counter_module, "_PCT_CTR_ABI_VERSION");
	if (PCT_CounterBEType == NULL || PyType_Check((PyObject *)PCT_CounterBEType) < 0 ||
		 PCT_CounterLEType == NULL || PyType_Check((PyObject *)PCT_CounterLEType) < 0 ||
		 abiver == NULL || PyInt_CheckExact(abiver) < 0 || PyInt_AS_LONG(abiver) != PCT_CTR_ABI_VERSION)
	{
		PyErr_SetString(PyExc_ImportError, "Crypto.Util._counter ABI mismatch.  Was PyCrypto incorrectly compiled?");
		goto errout;
	}

	/* Create __all__ (to help generate documentation) */
	__all__ = PyList_New(10);
	if (__all__ == NULL)
		goto errout;
	PyList_SetItem(__all__, 0, PyString_FromString(_MODULE_STRING));	/* This is the ALGType object */
	PyList_SetItem(__all__, 1, PyString_FromString("new"));
	PyList_SetItem(__all__, 2, PyString_FromString("MODE_ECB"));
	PyList_SetItem(__all__, 3, PyString_FromString("MODE_CBC"));
	PyList_SetItem(__all__, 4, PyString_FromString("MODE_CFB"));
	PyList_SetItem(__all__, 5, PyString_FromString("MODE_PGP"));
	PyList_SetItem(__all__, 6, PyString_FromString("MODE_OFB"));
	PyList_SetItem(__all__, 7, PyString_FromString("MODE_CTR"));
	PyList_SetItem(__all__, 8, PyString_FromString("block_size"));
	PyList_SetItem(__all__, 9, PyString_FromString("key_size"));
	PyObject_SetAttrString(m, "__all__", __all__);

out:
	/* Final error check */
	if (m == NULL && !PyErr_Occurred()) {
		PyErr_SetString(PyExc_ImportError, "can't initialize module");
		goto errout;
	}

	/* Free local objects here */
	Py_CLEAR(abiver);
	Py_CLEAR(__all__);

	/* Return */
#ifdef IS_PY3K
	return m;
#else
	return;
#endif

errout:
	/* Free the module and other global objects here */
	Py_CLEAR(m);
	Py_CLEAR(_counter_module);
	Py_CLEAR(PCT_CounterBEType);
	Py_CLEAR(PCT_CounterLEType);
	goto out;
}