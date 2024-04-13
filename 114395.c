u32 gf_isom_has_track_reference(GF_ISOFile *movie, u32 trackNumber, u32 referenceType, GF_ISOTrackID refTrackID)
{
	u32 i;
	GF_TrackBox *trak;
	GF_TrackReferenceTypeBox *dpnd;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return 0;
	if (!trak->References) return 0;

	dpnd = NULL;
	if ( (movie->LastError = Track_FindRef(trak, referenceType, &dpnd)) ) return 0;
	if (!dpnd) return 0;
	for (i=0; i<dpnd->trackIDCount; i++) {
		if (dpnd->trackIDs[i]==refTrackID) return i+1;
	}
	return 0;
}