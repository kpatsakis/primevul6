exif_entry_ref (ExifEntry *e)
{
	if (!e) return;

	e->priv->ref_count++;
}