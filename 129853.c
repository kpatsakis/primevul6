MonoArray*
mono_reflection_get_custom_attrs_blob (MonoReflectionAssembly *assembly, MonoObject *ctor, MonoArray *ctorArgs, MonoArray *properties, MonoArray *propValues, MonoArray *fields, MonoArray* fieldValues) 
{
	MonoArray *result;
	MonoMethodSignature *sig;
	MonoObject *arg;
	char *buffer, *p;
	guint32 buflen, i;

	MONO_ARCH_SAVE_REGS;

	if (strcmp (ctor->vtable->klass->name, "MonoCMethod")) {
		/* sig is freed later so allocate it in the heap */
		sig = ctor_builder_to_signature (NULL, (MonoReflectionCtorBuilder*)ctor);
	} else {
		sig = mono_method_signature (((MonoReflectionMethod*)ctor)->method);
	}

	g_assert (mono_array_length (ctorArgs) == sig->param_count);
	buflen = 256;
	p = buffer = g_malloc (buflen);
	/* write the prolog */
	*p++ = 1;
	*p++ = 0;
	for (i = 0; i < sig->param_count; ++i) {
		arg = mono_array_get (ctorArgs, MonoObject*, i);
		encode_cattr_value (assembly->assembly, buffer, p, &buffer, &p, &buflen, sig->params [i], arg, NULL);
	}
	i = 0;
	if (properties)
		i += mono_array_length (properties);
	if (fields)
		i += mono_array_length (fields);
	*p++ = i & 0xff;
	*p++ = (i >> 8) & 0xff;
	if (properties) {
		MonoObject *prop;
		for (i = 0; i < mono_array_length (properties); ++i) {
			MonoType *ptype;
			char *pname;

			prop = mono_array_get (properties, gpointer, i);
			get_prop_name_and_type (prop, &pname, &ptype);
			*p++ = 0x54; /* PROPERTY signature */
			encode_named_val (assembly, buffer, p, &buffer, &p, &buflen, ptype, pname, (MonoObject*)mono_array_get (propValues, gpointer, i));
			g_free (pname);
		}
	}

	if (fields) {
		MonoObject *field;
		for (i = 0; i < mono_array_length (fields); ++i) {
			MonoType *ftype;
			char *fname;

			field = mono_array_get (fields, gpointer, i);
			get_field_name_and_type (field, &fname, &ftype);
			*p++ = 0x53; /* FIELD signature */
			encode_named_val (assembly, buffer, p, &buffer, &p, &buflen, ftype, fname, (MonoObject*)mono_array_get (fieldValues, gpointer, i));
			g_free (fname);
		}
	}

	g_assert (p - buffer <= buflen);
	buflen = p - buffer;
	result = mono_array_new (mono_domain_get (), mono_defaults.byte_class, buflen);
	p = mono_array_addr (result, char, 0);
	memcpy (p, buffer, buflen);
	g_free (buffer);
	if (strcmp (ctor->vtable->klass->name, "MonoCMethod"))
		g_free (sig);
	return result;