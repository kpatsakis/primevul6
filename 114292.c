GF_Err gf_isom_get_current_top_box_offset(GF_ISOFile *movie, u64 *current_top_box_offset)
{
	if (!movie || !movie->moov || !current_top_box_offset) return GF_BAD_PARAM;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	*current_top_box_offset = movie->current_top_box_start;
	return GF_OK;
#else
	return GF_NOT_SUPPORTED;
#endif
}