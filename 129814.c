mono_image_fill_export_table_from_type_forwarders (MonoReflectionAssemblyBuilder *assemblyb, MonoDynamicImage *assembly)
{
	MonoClass *klass;
	int i;

	if (!assemblyb->type_forwarders)
		return;

	for (i = 0; i < mono_array_length (assemblyb->type_forwarders); ++i) {
		MonoReflectionType *t = mono_array_get (assemblyb->type_forwarders, MonoReflectionType *, i);
		MonoType *type;
		if (!t)
			continue;

		type = mono_reflection_type_get_handle (t);
		g_assert (type);

		klass = mono_class_from_mono_type (type);

		add_exported_type (assemblyb, assembly, klass, 0);
	}
}