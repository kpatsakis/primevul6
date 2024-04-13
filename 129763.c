mono_image_get_method_on_inst_token (MonoDynamicImage *assembly, MonoReflectionMethodOnTypeBuilderInst *m, gboolean create_methodspec)
{
	guint32 sig, token = 0;
	MonoType *type;
	MonoClass *klass;

	if (m->method_args) {
		MonoMethod *inflated;

		inflated = mono_reflection_method_on_tb_inst_get_handle (m);
		if (create_methodspec)
			token = mono_image_get_methodspec_token (assembly, inflated);
		else
			token = mono_image_get_inflated_method_token (assembly, inflated);
		return token;
	}

	token = GPOINTER_TO_UINT (mono_g_hash_table_lookup (assembly->handleref_managed, m));
	if (token)
		return token;

	if (is_sre_method_builder (mono_object_class (m->mb))) {
		MonoReflectionMethodBuilder *mb = (MonoReflectionMethodBuilder *)m->mb;
		MonoGenericClass *gclass;
		ReflectionMethodBuilder rmb;
		char *name;

		type = mono_reflection_type_get_handle ((MonoReflectionType*)m->inst);
		klass = mono_class_from_mono_type (type);
		gclass = type->data.generic_class;
		g_assert (gclass->is_dynamic);

		reflection_methodbuilder_from_method_builder (&rmb, mb);

		name = mono_string_to_utf8 (rmb.name);

		sig = method_builder_encode_signature (assembly, &rmb);

		token = mono_image_get_memberref_token (assembly, &klass->byval_arg, name, sig);
		g_free (name);		
	} else if (is_sr_mono_method (mono_object_class (m->mb))) {
		MonoMethod *mm = ((MonoReflectionMethod *)m->mb)->method;

		type = mono_reflection_type_get_handle ((MonoReflectionType*)m->inst);
		klass = mono_class_from_mono_type (type);

		sig = method_encode_signature (assembly, mono_method_signature (mm));
		token = mono_image_get_memberref_token (assembly, &klass->byval_arg, mm->name, sig);
	} else {
		char *name = mono_type_get_full_name (mono_object_class (m->mb));
		g_error ("mono_image_get_method_on_inst_token: don't know how to handle %s", name);
	}

	mono_g_hash_table_insert (assembly->handleref_managed, m, GUINT_TO_POINTER (token));
	return token;
}