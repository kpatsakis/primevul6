exif_entry_unref (ExifEntry *e)
{
	if (!e) return;

	e->priv->ref_count--;
	if (!e->priv->ref_count)
		exif_entry_free (e);
}