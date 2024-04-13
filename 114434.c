u32 gf_isom_get_avg_sample_size(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->SampleSize) return 0;

	if ( trak->Media->information->sampleTable->SampleSize->sampleSize)
		return trak->Media->information->sampleTable->SampleSize->sampleSize;

	if (!trak->Media->information->sampleTable->SampleSize->total_samples) return 0;
	return (u32) (trak->Media->information->sampleTable->SampleSize->total_size / trak->Media->information->sampleTable->SampleSize->total_samples);
}