mono_get_dbnull_object (MonoDomain *domain)
{
	MonoObject *obj;
	static MonoClassField *dbnull_value_field = NULL;
	
	if (!dbnull_value_field) {
		MonoClass *dbnull_klass;
		dbnull_klass = mono_class_from_name (mono_defaults.corlib, "System", "DBNull");
		mono_class_init (dbnull_klass);
		dbnull_value_field = mono_class_get_field_from_name (dbnull_klass, "Value");
		g_assert (dbnull_value_field);
	}
	obj = mono_field_get_value_object (domain, dbnull_value_field, NULL); 
	g_assert (obj);
	return obj;
}