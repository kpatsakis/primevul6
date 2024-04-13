void gf_isom_set_last_error(GF_ISOFile *movie, GF_Err error)
{
	if (!movie) {
		MP4_API_IO_Err = error;
	} else {
		movie->LastError = error;
	}
}