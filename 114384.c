u32 gf_isom_get_constant_sample_duration(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->TimeToSample) return 0;
	if (trak->Media->information->sampleTable->TimeToSample->nb_entries != 1) return 0;
	return trak->Media->information->sampleTable->TimeToSample->entries[0].sampleDelta;
}