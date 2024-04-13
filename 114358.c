void gf_isom_reset_fragment_info(GF_ISOFile *movie, Bool keep_sample_count)
{
	u32 i;
	if (!movie) return;
	for (i=0; i<gf_list_count(movie->moov->trackList); i++) {
		GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(movie->moov->trackList, i);
		trak->Media->information->sampleTable->SampleSize->sampleCount = 0;
#ifdef GPAC_DISABLE_ISOM_FRAGMENTS
	}
#else
		//do not reset tfdt for LL-HLS case where parts do not contain a TFDT
		//trak->dts_at_seg_start = 0;
		if (!keep_sample_count)
			trak->sample_count_at_seg_start = 0;
	}