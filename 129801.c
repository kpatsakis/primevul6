static void
encode_named_val (MonoReflectionAssembly *assembly, char *buffer, char *p, char **retbuffer, char **retp, guint32 *buflen, MonoType *type, char *name, MonoObject *value)
{
	int len;
	/* Preallocate a large enough buffer */
	if (type->type == MONO_TYPE_VALUETYPE && type->data.klass->enumtype) {
		char *str = type_get_qualified_name (type, NULL);
		len = strlen (str);
		g_free (str);
	} else if (type->type == MONO_TYPE_SZARRAY && type->data.klass->enumtype) {
		char *str = type_get_qualified_name (&type->data.klass->byval_arg, NULL);
		len = strlen (str);
		g_free (str);
	} else {
		len = 0;
	}
	len += strlen (name);

	if ((p-buffer) + 20 + len >= *buflen) {
		char *newbuf;
		*buflen *= 2;
		*buflen += len;
		newbuf = g_realloc (buffer, *buflen);
		p = newbuf + (p-buffer);
		buffer = newbuf;
	}

	encode_field_or_prop_type (type, p, &p);

	len = strlen (name);
	mono_metadata_encode_value (len, p, &p);
	memcpy (p, name, len);
	p += len;
	encode_cattr_value (assembly->assembly, buffer, p, &buffer, &p, buflen, type, value, NULL);
	*retp = p;
	*retbuffer = buffer;