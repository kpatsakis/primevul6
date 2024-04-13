u8 gf_isom_get_mode(GF_ISOFile *the_file)
{
	if (!the_file) return 0;
	return the_file->openMode;
}