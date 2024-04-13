exif_entry_new_mem (ExifMem *mem)
{
	ExifEntry *e = NULL;

	e = exif_mem_alloc (mem, sizeof (ExifEntry));
	if (!e) return NULL;
	e->priv = exif_mem_alloc (mem, sizeof (ExifEntryPrivate));
	if (!e->priv) { exif_mem_free (mem, e); return NULL; }
	e->priv->ref_count = 1;

	e->priv->mem = mem;
	exif_mem_ref (mem);

	return e;
}