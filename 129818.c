mono_field_get_object (MonoDomain *domain, MonoClass *klass, MonoClassField *field)
{
	MonoReflectionField *res;
	static MonoClass *monofield_klass;

	CHECK_OBJECT (MonoReflectionField *, field, klass);
	if (!monofield_klass)
		monofield_klass = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MonoField");
	res = (MonoReflectionField *)mono_object_new (domain, monofield_klass);
	res->klass = klass;
	res->field = field;
	MONO_OBJECT_SETREF (res, name, mono_string_new (domain, mono_field_get_name (field)));

	if (is_field_on_inst (field)) {
		res->attrs = get_field_on_inst_generic_type (field)->attrs;
		MONO_OBJECT_SETREF (res, type, mono_type_get_object (domain, field->type));
	} else {
		if (field->type)
			MONO_OBJECT_SETREF (res, type, mono_type_get_object (domain, field->type));
		res->attrs = mono_field_get_flags (field);
	}
	CACHE_OBJECT (MonoReflectionField *, field, res, klass);
}