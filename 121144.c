ALG_Decrypt(ALGobject *self, PyObject *args)
{
	unsigned char *buffer, *str;
	unsigned char temp[BLOCK_SIZE];
	int i, j, len;
	PyObject *result;

	/* CTR and OFB mode decryption is identical to encryption */
	if (self->mode == MODE_CTR || self->mode == MODE_OFB)
		return ALG_Encrypt(self, args);

	if (!PyArg_Parse(args, "s#", &str, &len))
		return NULL;
	if (len==0)			/* Handle empty string */
	{
		return PyBytes_FromStringAndSize(NULL, 0);
	}
	if ( (len % BLOCK_SIZE) !=0 && (self->mode!=MODE_CFB))
	{
		PyErr_Format(PyExc_ValueError, 
			     "Input strings must be "
			     "a multiple of %i in length",
			     BLOCK_SIZE);
		return NULL;
	}
	if (self->mode == MODE_CFB && 
	    (len % (self->segment_size/8) !=0)) {
		PyErr_Format(PyExc_ValueError, 
			     "Input strings must be a multiple of "
			     "the segment size %i in length",
			     self->segment_size/8);
		return NULL;
	}
	buffer=malloc(len);
	if (buffer==NULL) 
	{
		PyErr_SetString(PyExc_MemoryError, 
				"No memory available in " _MODULE_STRING
				" decrypt");
		return NULL;
	}
	Py_BEGIN_ALLOW_THREADS;
	switch(self->mode)
	{
	case(MODE_ECB):      
		for(i=0; i<len; i+=BLOCK_SIZE) 
		{
			block_decrypt(&(self->st), str+i, buffer+i);
		}
		break;

	case(MODE_CBC):      
		for(i=0; i<len; i+=BLOCK_SIZE) 
		{
			memcpy(self->oldCipher, self->IV, BLOCK_SIZE);
			block_decrypt(&(self->st), str+i, temp);
			for(j=0; j<BLOCK_SIZE; j++) 
			{
				buffer[i+j]=temp[j]^self->IV[j];
				self->IV[j]=str[i+j];
			}
		}
		break;

	case(MODE_CFB):      
		for(i=0; i<len; i+=self->segment_size/8) 
		{
			block_encrypt(&(self->st), self->IV, temp);
			for (j=0; j<self->segment_size/8; j++) {
				buffer[i+j] = str[i+j]^temp[j];
			}
			if (self->segment_size == BLOCK_SIZE * 8) {
				/* s == b: segment size is identical to 
				   the algorithm block size */
				memcpy(self->IV, str + i, BLOCK_SIZE);
			}
			else if ((self->segment_size % 8) == 0) {
				int sz = self->segment_size/8;
				memmove(self->IV, self->IV + sz, 
					BLOCK_SIZE-sz);
				memcpy(self->IV + BLOCK_SIZE - sz, str + i, 
				       sz);
			}
			else {
				/* segment_size is not a multiple of 8; 
				   currently this can't happen */
			}
		}
		break;

	default:
		Py_BLOCK_THREADS;
		PyErr_Format(PyExc_SystemError, 
			     "Unknown ciphertext feedback mode %i; "
			     "this shouldn't happen",
			     self->mode);
		free(buffer);
		return NULL;
	}
	Py_END_ALLOW_THREADS;
	result=PyBytes_FromStringAndSize((char *) buffer, len);
	free(buffer);
	return(result);
}