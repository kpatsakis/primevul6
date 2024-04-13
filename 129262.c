exif_entry_alloc (ExifEntry *e, unsigned int i)
{
	void *d;
	ExifLog *l = NULL;

	if (!e || !e->priv || !i) return NULL;

	d = exif_mem_alloc (e->priv->mem, i);
	if (d) return d;

	if (e->parent && e->parent->parent)
		l = exif_data_get_log (e->parent->parent);
	EXIF_LOG_NO_MEMORY (l, "ExifEntry", i);
	return NULL;
}