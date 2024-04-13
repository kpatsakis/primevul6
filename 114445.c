u32 gf_isom_get_edits_count(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 0;

	if (!trak->editBox || !trak->editBox->editList) return 0;
	return gf_list_count(trak->editBox->editList->entryList);
}