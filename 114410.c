u8 gf_isom_is_track_enabled(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);

	if (!trak || !trak->Header) return 2;
	return (trak->Header->flags & 1) ? 1 : 0;
}