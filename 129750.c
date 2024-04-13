mono_image_get_memberref_token (MonoDynamicImage *assembly, MonoType *type, const char *name, guint32 sig)
{
	guint32 parent = mono_image_typedef_or_ref (assembly, type);
	return mono_image_add_memberef_row (assembly, parent, name, sig);
}