type_get_qualified_name (MonoType *type, MonoAssembly *ass) {
	MonoClass *klass;
	MonoAssembly *ta;

	klass = mono_class_from_mono_type (type);
	if (!klass) 
		return mono_type_get_name_full (type, MONO_TYPE_NAME_FORMAT_REFLECTION);
	ta = klass->image->assembly;
	if (ta->dynamic || (ta == ass)) {
		if (klass->generic_class || klass->generic_container)
			/* For generic type definitions, we want T, while REFLECTION returns T<K> */
			return mono_type_get_name_full (type, MONO_TYPE_NAME_FORMAT_FULL_NAME);
		else
			return mono_type_get_name_full (type, MONO_TYPE_NAME_FORMAT_REFLECTION);
	}

	return mono_type_get_name_full (type, MONO_TYPE_NAME_FORMAT_ASSEMBLY_QUALIFIED);
}