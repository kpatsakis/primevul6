Bool gf_isom_has_sample_dependency(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_FALSE;
	if (!trak->Media->information->sampleTable->SampleDep) return GF_FALSE;
	return GF_TRUE;
}