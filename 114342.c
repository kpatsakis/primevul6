u32 gf_isom_get_sample_count(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->SampleSize) return 0;
	return trak->Media->information->sampleTable->SampleSize->sampleCount
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	       + trak->sample_count_at_seg_start
#endif
	       ;
}