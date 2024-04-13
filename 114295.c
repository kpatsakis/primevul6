u32 gf_isom_get_sample_duration(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber)
{
	u32 dur;
	u64 dts;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !sampleNumber) return 0;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sampleNumber<=trak->sample_count_at_seg_start) return 0;
	sampleNumber -= trak->sample_count_at_seg_start;
#endif

	stbl_GetSampleDTS_and_Duration(trak->Media->information->sampleTable->TimeToSample, sampleNumber, &dts, &dur);
	return dur;
}