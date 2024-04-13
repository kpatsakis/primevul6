u32 gf_isom_get_track_count(GF_ISOFile *movie)
{
	if (!movie || !movie->moov) return 0;

	if (!movie->moov->trackList) {
		movie->LastError = GF_ISOM_INVALID_FILE;
		return 0;
	}
	return gf_list_count(movie->moov->trackList);
}