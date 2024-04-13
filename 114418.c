GF_Err gf_isom_reset_data_offset(GF_ISOFile *movie, u64 *top_box_start)
{
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	u32 i, count;
	if (!movie || !movie->moov) return GF_BAD_PARAM;
	if (top_box_start) *top_box_start = movie->current_top_box_start;
	movie->current_top_box_start = 0;
	movie->NextMoofNumber = 0;
	if (movie->moov->mvex && movie->single_moof_mode) {
		movie->single_moof_state = 0;
	}
	count = gf_list_count(movie->moov->trackList);
	for (i=0; i<count; i++) {
		GF_TrackBox *tk = gf_list_get(movie->moov->trackList, i);
		tk->first_traf_merged = GF_FALSE;
	}
#endif
	return GF_OK;
}