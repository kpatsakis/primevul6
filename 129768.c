mono_reflection_resolve_custom_attribute_data (MonoReflectionMethod *ref_method, MonoReflectionAssembly *assembly, gpointer data, guint32 len, MonoArray **ctor_args, MonoArray **named_args)
{
	MonoDomain *domain;
	MonoArray *typedargs, *namedargs;
	MonoImage *image;
	MonoMethod *method;
	CattrNamedArg *arginfo;
	int i;

	*ctor_args = NULL;
	*named_args = NULL;

	if (len == 0)
		return;

	image = assembly->assembly->image;
	method = ref_method->method;
	domain = mono_object_domain (ref_method);

	if (!mono_class_init (method->klass))
		mono_raise_exception (mono_class_get_exception_for_failure (method->klass));

	mono_reflection_create_custom_attr_data_args (image, method, data, len, &typedargs, &namedargs, &arginfo);
	if (mono_loader_get_last_error ())
		mono_raise_exception (mono_loader_error_prepare_exception (mono_loader_get_last_error ()));

	if (!typedargs || !namedargs)
		return;

	for (i = 0; i < mono_method_signature (method)->param_count; ++i) {
		MonoObject *obj = mono_array_get (typedargs, MonoObject*, i);
		MonoObject *typedarg;

		typedarg = create_cattr_typed_arg (mono_method_signature (method)->params [i], obj);
		mono_array_setref (typedargs, i, typedarg);
	}

	for (i = 0; i < mono_array_length (namedargs); ++i) {
		MonoObject *obj = mono_array_get (namedargs, MonoObject*, i);
		MonoObject *typedarg, *namedarg, *minfo;

		if (arginfo [i].prop)
			minfo = (MonoObject*)mono_property_get_object (domain, NULL, arginfo [i].prop);
		else
			minfo = (MonoObject*)mono_field_get_object (domain, NULL, arginfo [i].field);

		typedarg = create_cattr_typed_arg (arginfo [i].type, obj);
		namedarg = create_cattr_named_arg (minfo, typedarg);

		mono_array_setref (namedargs, i, namedarg);
	}

	*ctor_args = typedargs;
	*named_args = namedargs;
}