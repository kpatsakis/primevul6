Bool gf_isom_has_root_od(GF_ISOFile *movie)
{
	if (!movie || !movie->moov || !movie->moov->iods || !movie->moov->iods->descriptor) return GF_FALSE;
	return GF_TRUE;
}