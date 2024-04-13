GF_Err gf_isom_get_bs(GF_ISOFile *movie, GF_BitStream **out_bs)
{
#ifndef GPAC_DISABLE_ISOM_WRITE
	if (!movie || movie->openMode != GF_ISOM_OPEN_WRITE || !movie->editFileMap) //memory mode
		return GF_NOT_SUPPORTED;

	if (movie->segment_bs)
		*out_bs = movie->segment_bs;
	else
		*out_bs = movie->editFileMap->bs;

	if (movie->moof)
		movie->moof->fragment_offset = 0;

	return GF_OK;
#else
	return GF_NOT_SUPPORTED;
#endif
}