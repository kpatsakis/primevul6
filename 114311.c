s32 gf_isom_get_reference_count(GF_ISOFile *movie, u32 trackNumber, u32 referenceType)
{
	GF_TrackBox *trak;
	GF_TrackReferenceTypeBox *dpnd;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return -1;
	if (!trak->References) return 0;
	if (movie->openMode == GF_ISOM_OPEN_WRITE) {
		movie->LastError = GF_ISOM_INVALID_MODE;
		return -1;
	}

	dpnd = NULL;
	if ( (movie->LastError = Track_FindRef(trak, referenceType, &dpnd)) ) return -1;
	if (!dpnd) return 0;
	return dpnd->trackIDCount;
}