u32 gf_isom_get_sample_size(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber)
{
	u32 size = 0;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !sampleNumber) return 0;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sampleNumber<=trak->sample_count_at_seg_start) return 0;
	sampleNumber -= trak->sample_count_at_seg_start;
#endif
	stbl_GetSampleSize(trak->Media->information->sampleTable->SampleSize, sampleNumber, &size);
	return size;
}