register_assembly (MonoDomain *domain, MonoReflectionAssembly *res, MonoAssembly *assembly)
{
	CACHE_OBJECT (MonoReflectionAssembly *, assembly, res, NULL);
}