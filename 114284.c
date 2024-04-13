GF_Err gf_isom_get_handler_name(GF_ISOFile *the_file, u32 trackNumber, const char **outName)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !outName) return GF_BAD_PARAM;
	*outName = trak->Media->handler->nameUTF8;
	return GF_OK;
}