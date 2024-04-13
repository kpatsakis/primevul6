mono_image_get_ctor_info (MonoDomain *domain, MonoReflectionCtorBuilder *mb, MonoDynamicImage *assembly)
{
	ReflectionMethodBuilder rmb;

	reflection_methodbuilder_from_ctor_builder (&rmb, mb);

	mono_image_basic_method (&rmb, assembly);
	mb->table_idx = *rmb.table_idx;
}