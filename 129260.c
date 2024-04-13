exif_entry_realloc (ExifEntry *e, void *d_orig, unsigned int i)
{
	void *d;
	ExifLog *l = NULL;

	if (!e || !e->priv) return NULL;

	if (!i) { exif_mem_free (e->priv->mem, d_orig); return NULL; }

	d = exif_mem_realloc (e->priv->mem, d_orig, i);
	if (d) return d;

	if (e->parent && e->parent->parent)
		l = exif_data_get_log (e->parent->parent);
	EXIF_LOG_NO_MEMORY (l, "ExifEntry", i);
	return NULL;
}