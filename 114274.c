u32 gf_isom_get_media_timescale(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !trak->Media->mediaHeader) return 0;
	return trak->Media->mediaHeader->timeScale;
}