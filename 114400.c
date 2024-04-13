GF_Err gf_isom_close(GF_ISOFile *movie)
{
	GF_Err e=GF_OK;
	if (movie == NULL) return GF_ISOM_INVALID_FILE;
	e = gf_isom_write(movie);
	//free and return;
	gf_isom_delete_movie(movie);
	return e;
}