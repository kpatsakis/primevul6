u32 gf_isom_get_media_type(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	return (trak->Media && trak->Media->handler) ? trak->Media->handler->handlerType : 0;
}