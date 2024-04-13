GF_Err gf_isom_get_track_creation_time(GF_ISOFile *movie, u32 trackNumber, u64 *creationTime, u64 *modificationTime)
{
	GF_TrackBox *trak;
	if (!movie || !movie->moov) return GF_BAD_PARAM;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return 0;

	if (creationTime) *creationTime = trak->Media->mediaHeader->creationTime;
	if (creationTime) *modificationTime = trak->Media->mediaHeader->modificationTime;
	return GF_OK;
}