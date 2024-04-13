u32 gf_isom_get_track_by_id(GF_ISOFile *the_file, GF_ISOTrackID trackID)
{
	u32 count;
	u32 i;
	if (the_file == NULL) return 0;

	count = gf_isom_get_track_count(the_file);
	if (!count) return 0;
	for (i = 0; i < count; i++) {
		GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, i+1);
		if (!trak || !trak->Header) return 0;
		if (trak->Header->trackID == trackID) return i+1;
	}
	return 0;
}