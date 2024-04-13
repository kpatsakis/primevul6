GF_ISOSample *gf_isom_get_sample_ex(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber, u32 *sampleDescriptionIndex, GF_ISOSample *static_sample, u64 *data_offset)
{
	GF_Err e;
	u32 descIndex;
	GF_TrackBox *trak;
	GF_ISOSample *samp;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return NULL;

	if (!sampleNumber) return NULL;
	if (static_sample) {
		samp = static_sample;
		if (static_sample->dataLength && !static_sample->alloc_size)
			static_sample->alloc_size = static_sample->dataLength;
	} else {
		samp = gf_isom_sample_new();
	}
	if (!samp) return NULL;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sampleNumber<=trak->sample_count_at_seg_start)
		return NULL;
	sampleNumber -= trak->sample_count_at_seg_start;
#endif

	e = Media_GetSample(trak->Media, sampleNumber, &samp, &descIndex, GF_FALSE, data_offset);
	if (static_sample && !static_sample->alloc_size)
		static_sample->alloc_size = static_sample->dataLength;

	if (e) {
		gf_isom_set_last_error(the_file, e);
		if (!static_sample) gf_isom_sample_del(&samp);
		return NULL;
	}
	if (sampleDescriptionIndex) *sampleDescriptionIndex = descIndex;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (samp) samp->DTS += trak->dts_at_seg_start;
#endif

	return samp;
}