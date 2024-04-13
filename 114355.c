Bool gf_isom_has_movie(GF_ISOFile *file)
{
	if (file && file->moov) return GF_TRUE;
	return GF_FALSE;
}