u64 gf_isom_get_media_duration(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return 0;


#ifndef GPAC_DISABLE_ISOM_WRITE

	/*except in dump mode always recompute the duration*/
	if (movie->openMode != GF_ISOM_OPEN_READ_DUMP) {
		if ( (movie->LastError = Media_SetDuration(trak)) ) return 0;
	}

#endif

	return trak->Media->mediaHeader->duration;
}