exif_entry_new (void)
{
	ExifMem *mem = exif_mem_new_default ();
	ExifEntry *e = exif_entry_new_mem (mem);

	exif_mem_unref (mem);

	return e;
}