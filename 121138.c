ALGdealloc(PyObject *ptr)
{		
	ALGobject *self = (ALGobject *)ptr;

	/* Overwrite the contents of the object */
	Py_XDECREF(self->counter);
	self->counter = NULL;
	memset(self->IV, 0, BLOCK_SIZE);
	memset(self->oldCipher, 0, BLOCK_SIZE);
	memset((char*)&(self->st), 0, sizeof(block_state));
	self->mode = self->count = self->segment_size = 0;
	PyObject_Del(ptr);
}