create_cattr_named_arg (void *minfo, MonoObject *typedarg)
{
	static MonoClass *klass;
	static MonoMethod *ctor;
	MonoObject *retval;
	void *unboxed, *params [2];

	if (!klass)
		klass = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "CustomAttributeNamedArgument");
	if (!ctor)
		ctor = mono_class_get_method_from_name (klass, ".ctor", 2);

	params [0] = minfo;
	params [1] = typedarg;
	retval = mono_object_new (mono_domain_get (), klass);
	unboxed = mono_object_unbox (retval);
	mono_runtime_invoke (ctor, unboxed, params, NULL);

	return retval;
}