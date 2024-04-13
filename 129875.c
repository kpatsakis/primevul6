mono_assembly_get_object (MonoDomain *domain, MonoAssembly *assembly)
{
	static MonoClass *assembly_type;
	MonoReflectionAssembly *res;
	
	CHECK_OBJECT (MonoReflectionAssembly *, assembly, NULL);
	if (!assembly_type) {
		MonoClass *class = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MonoAssembly");
		if (class == NULL)
			class = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "Assembly");
		g_assert (class);
		assembly_type = class;
	}
	res = (MonoReflectionAssembly *)mono_object_new (domain, assembly_type);
	res->assembly = assembly;

	CACHE_OBJECT (MonoReflectionAssembly *, assembly, res, NULL);
}