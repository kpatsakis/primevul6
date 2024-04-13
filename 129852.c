create_custom_attr (MonoImage *image, MonoMethod *method, const guchar *data, guint32 len)
{
	const char *p = (const char*)data;
	const char *named;
	guint32 i, j, num_named;
	MonoObject *attr;
	void *params_buf [32];
	void **params;
	MonoMethodSignature *sig;

	mono_class_init (method->klass);

	if (!mono_verifier_verify_cattr_content (image, method, data, len, NULL))
		return NULL;

	if (len == 0) {
		attr = mono_object_new (mono_domain_get (), method->klass);
		mono_runtime_invoke (method, attr, NULL, NULL);
		return attr;
	}

	if (len < 2 || read16 (p) != 0x0001) /* Prolog */
		return NULL;

	/*g_print ("got attr %s\n", method->klass->name);*/

	sig = mono_method_signature (method);
	if (sig->param_count < 32)
		params = params_buf;
	else
		/* Allocate using GC so it gets GC tracking */
		params = mono_gc_alloc_fixed (sig->param_count * sizeof (void*), NULL);

	/* skip prolog */
	p += 2;
	for (i = 0; i < mono_method_signature (method)->param_count; ++i) {
		params [i] = load_cattr_value (image, mono_method_signature (method)->params [i], p, &p);
	}

	named = p;
	attr = mono_object_new (mono_domain_get (), method->klass);
	mono_runtime_invoke (method, attr, params, NULL);
	free_param_data (method->signature, params);
	num_named = read16 (named);
	named += 2;
	for (j = 0; j < num_named; j++) {
		gint name_len;
		char *name, named_type, data_type;
		named_type = *named++;
		data_type = *named++; /* type of data */
		if (data_type == MONO_TYPE_SZARRAY)
			data_type = *named++;
		if (data_type == MONO_TYPE_ENUM) {
			gint type_len;
			char *type_name;
			type_len = mono_metadata_decode_blob_size (named, &named);
			type_name = g_malloc (type_len + 1);
			memcpy (type_name, named, type_len);
			type_name [type_len] = 0;
			named += type_len;
			/* FIXME: lookup the type and check type consistency */
			g_free (type_name);
		}
		name_len = mono_metadata_decode_blob_size (named, &named);
		name = g_malloc (name_len + 1);
		memcpy (name, named, name_len);
		name [name_len] = 0;
		named += name_len;
		if (named_type == 0x53) {
			MonoClassField *field = mono_class_get_field_from_name (mono_object_class (attr), name);
			void *val = load_cattr_value (image, field->type, named, &named);
			mono_field_set_value (attr, field, val);
			if (!type_is_reference (field->type))
				g_free (val);
		} else if (named_type == 0x54) {
			MonoProperty *prop;
			void *pparams [1];
			MonoType *prop_type;

			prop = mono_class_get_property_from_name (mono_object_class (attr), name);
			/* can we have more that 1 arg in a custom attr named property? */
			prop_type = prop->get? mono_method_signature (prop->get)->ret :
			     mono_method_signature (prop->set)->params [mono_method_signature (prop->set)->param_count - 1];
			pparams [0] = load_cattr_value (image, prop_type, named, &named);
			mono_property_set_value (prop, attr, pparams, NULL);
			if (!type_is_reference (prop_type))
				g_free (pparams [0]);
		}
		g_free (name);
	}

	if (params != params_buf)
		mono_gc_free_fixed (params);

	return attr;
}