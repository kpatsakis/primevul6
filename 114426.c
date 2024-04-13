u32 gf_isom_has_time_offset(GF_ISOFile *the_file, u32 trackNumber)
{
	u32 i;
	GF_CompositionOffsetBox *ctts;
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media->information->sampleTable->CompositionOffset) return 0;

	//return true at the first offset found
	ctts = trak->Media->information->sampleTable->CompositionOffset;
	for (i=0; i<ctts->nb_entries; i++) {
		if (ctts->entries[i].decodingOffset && ctts->entries[i].sampleCount) return ctts->version ? 2 : 1;
	}
	return 0;
}