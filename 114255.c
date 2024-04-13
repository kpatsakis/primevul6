u64 gf_isom_get_media_original_duration(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media || !trak->Media->mediaHeader) return 0;

	return trak->Media->mediaHeader->original_duration;
}