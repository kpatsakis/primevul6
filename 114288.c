GF_Err gf_isom_last_error(GF_ISOFile *the_file)
{
	if (!the_file) return MP4_API_IO_Err;
	return the_file->LastError;
}