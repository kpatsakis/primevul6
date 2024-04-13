GF_ISOTrackID gf_isom_get_track_id(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackBox *trak;
	if (!movie) return 0;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Header) return 0;
	return trak->Header->trackID;
}