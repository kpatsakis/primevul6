mono_image_set_wrappers_type (MonoReflectionModuleBuilder *moduleb, MonoReflectionType *type)
{
	MonoDynamicImage *image = moduleb->dynamic_image;

	g_assert (type->type);
	image->wrappers_type = mono_class_from_mono_type (type->type);
}