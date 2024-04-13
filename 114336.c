Bool gf_isom_has_time_offset_table(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media->information->sampleTable->CompositionOffset) return GF_FALSE;
	return GF_TRUE;
}