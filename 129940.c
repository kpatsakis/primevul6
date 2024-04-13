image_g_malloc0 (MonoImage *image, guint size)
{
	if (image)
		return mono_image_alloc0 (image, size);
	else
		return g_malloc0 (size);
}