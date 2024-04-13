MonoType*
mono_reflection_bind_generic_parameters (MonoReflectionType *type, int type_argc, MonoType **types)
{
	MonoClass *klass;
	MonoReflectionTypeBuilder *tb = NULL;
	gboolean is_dynamic = FALSE;
	MonoDomain *domain;
	MonoClass *geninst;

	mono_loader_lock ();

	domain = mono_object_domain (type);

	if (is_sre_type_builder (mono_object_class (type))) {
		tb = (MonoReflectionTypeBuilder *) type;

		is_dynamic = TRUE;
	} else if (is_sre_generic_instance (mono_object_class (type))) {
		MonoReflectionGenericClass *rgi = (MonoReflectionGenericClass *) type;
		MonoReflectionType *gtd = rgi->generic_type;

		if (is_sre_type_builder (mono_object_class (gtd))) {
			tb = (MonoReflectionTypeBuilder *)gtd;
			is_dynamic = TRUE;
		}
	}

	/* FIXME: fix the CreateGenericParameters protocol to avoid the two stage setup of TypeBuilders */
	if (tb && tb->generic_container)
		mono_reflection_create_generic_class (tb);

	klass = mono_class_from_mono_type (mono_reflection_type_get_handle (type));
	if (!klass->generic_container) {
		mono_loader_unlock ();
		return NULL;
	}

	if (klass->wastypebuilder) {
		tb = (MonoReflectionTypeBuilder *) mono_class_get_ref_info (klass);

		is_dynamic = TRUE;
	}

	mono_loader_unlock ();

	geninst = mono_class_bind_generic_parameters (klass, type_argc, types, is_dynamic);

	return &geninst->byval_arg;