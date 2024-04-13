u32 gf_isom_get_max_sample_delta(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->TimeToSample) return 0;

	return trak->Media->information->sampleTable->TimeToSample->max_ts_delta;
}