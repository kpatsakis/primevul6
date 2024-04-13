u32 gf_isom_get_sample_from_dts(GF_ISOFile *the_file, u32 trackNumber, u64 dts)
{
	GF_Err e;
	u32 sampleNumber, prevSampleNumber;
	GF_TrackBox *trak;
	GF_SampleTableBox *stbl;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 0;

	stbl = trak->Media->information->sampleTable;

	e = stbl_findEntryForTime(stbl, dts, 1, &sampleNumber, &prevSampleNumber);
	if (e) return 0;
	return sampleNumber;
}