u64 gf_isom_get_original_duration(GF_ISOFile *movie)
{
	if (!movie || !movie->moov|| !movie->moov->mvhd) return 0;
	return movie->moov->mvhd->original_duration;
}