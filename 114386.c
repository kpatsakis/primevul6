u32 gf_isom_get_track_flags(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 0;
	return trak->Header->flags;
}