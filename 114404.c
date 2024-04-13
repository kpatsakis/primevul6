u32 gf_isom_get_timescale(GF_ISOFile *movie)
{
	if (!movie || !movie->moov || !movie->moov->mvhd) return 0;
	return movie->moov->mvhd->timeScale;
}