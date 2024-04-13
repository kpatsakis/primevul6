Bool gf_isom_get_sample_sync(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber)
{
	GF_ISOSAPType is_rap;
	GF_Err e;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !sampleNumber) return GF_FALSE;

	if (! trak->Media->information->sampleTable->SyncSample) return GF_TRUE;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sampleNumber<=trak->sample_count_at_seg_start) return GF_FALSE;
	sampleNumber -= trak->sample_count_at_seg_start;
#endif
	e = stbl_GetSampleRAP(trak->Media->information->sampleTable->SyncSample, sampleNumber, &is_rap, NULL, NULL);
	if (e) return GF_FALSE;
	return is_rap ? GF_TRUE : GF_FALSE;
}