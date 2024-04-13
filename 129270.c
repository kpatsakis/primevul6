exif_entry_free (ExifEntry *e)
{
	if (!e) return;

	if (e->priv) {
		ExifMem *mem = e->priv->mem;
		if (e->data)
			exif_mem_free (mem, e->data);
		exif_mem_free (mem, e->priv);
		exif_mem_free (mem, e);
		exif_mem_unref (mem);
	}
}