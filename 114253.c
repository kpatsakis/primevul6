GF_Err gf_isom_get_sample_flags(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber, u32 *isLeading, u32 *dependsOn, u32 *dependedOn, u32 *redundant)
{
	GF_TrackBox *trak;
	*isLeading = 0;
	*dependsOn = 0;
	*dependedOn = 0;
	*redundant = 0;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->Media->information->sampleTable->SampleDep) return GF_BAD_PARAM;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sampleNumber <= trak->sample_count_at_seg_start)
		return GF_BAD_PARAM;
	sampleNumber -= trak->sample_count_at_seg_start;
#endif

	return stbl_GetSampleDepType(trak->Media->information->sampleTable->SampleDep, sampleNumber, isLeading, dependsOn, dependedOn, redundant);
}