u64 gf_isom_get_sample_dts(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber)
{
	u64 dts;
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 0;

	if (!sampleNumber) return 0;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sampleNumber<=trak->sample_count_at_seg_start) return 0;
	sampleNumber -= trak->sample_count_at_seg_start;
#endif
	if (stbl_GetSampleDTS(trak->Media->information->sampleTable->TimeToSample, sampleNumber, &dts) != GF_OK) return 0;
	return dts;
}