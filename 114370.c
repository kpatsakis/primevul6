u32 gf_isom_get_sample_description_index(GF_ISOFile *movie, u32 trackNumber, u64 for_time)
{
	u32 streamDescIndex;
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return 0;

	if ( (movie->LastError = Media_GetSampleDescIndex(trak->Media, for_time, &streamDescIndex)) ) {
		return 0;
	}
	return streamDescIndex;
}