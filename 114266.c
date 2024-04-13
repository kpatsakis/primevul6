s64 gf_isom_get_cts_to_dts_shift(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media->information->sampleTable->CompositionToDecode) return 0;
	return trak->Media->information->sampleTable->CompositionToDecode->compositionToDTSShift;
}