GF_ISOSample *gf_isom_get_sample_info_ex(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber, u32 *sampleDescriptionIndex, u64 *data_offset, GF_ISOSample *static_sample)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_ISOSample *samp;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return NULL;

	if (!sampleNumber) return NULL;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sampleNumber<=trak->sample_count_at_seg_start) return NULL;
	sampleNumber -= trak->sample_count_at_seg_start;
#endif
	if (static_sample) {
		samp = static_sample;
	} else {
		samp = gf_isom_sample_new();
		if (!samp) return NULL;
	}

	e = Media_GetSample(trak->Media, sampleNumber, &samp, sampleDescriptionIndex, GF_TRUE, data_offset);
	if (e) {
		gf_isom_set_last_error(the_file, e);
		if (!static_sample)
			gf_isom_sample_del(&samp);
		return NULL;
	}
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (samp) samp->DTS += trak->dts_at_seg_start;
#endif
	return samp;
}