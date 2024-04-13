GF_Err gf_isom_get_reference(GF_ISOFile *movie, u32 trackNumber, u32 referenceType, u32 referenceIndex, u32 *refTrack)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_TrackReferenceTypeBox *dpnd;
	GF_ISOTrackID refTrackNum;
	trak = gf_isom_get_track_from_file(movie, trackNumber);

	*refTrack = 0;
	if (!trak || !trak->References) return GF_BAD_PARAM;

	dpnd = NULL;
	e = Track_FindRef(trak, referenceType, &dpnd);
	if (e) return e;
	if (!dpnd) return GF_BAD_PARAM;

	if (!referenceIndex || (referenceIndex > dpnd->trackIDCount)) return GF_BAD_PARAM;

	//the spec allows a NULL reference
	//(ex, to force desync of a track, set a sync ref with ID = 0)
	if (dpnd->trackIDs[referenceIndex - 1] == 0) return GF_OK;

	refTrackNum = gf_isom_get_tracknum_from_id(movie->moov, dpnd->trackIDs[referenceIndex-1]);

	//if the track was not found, this means the file is broken !!!
	if (! refTrackNum) return GF_ISOM_INVALID_FILE;
	*refTrack = refTrackNum;
	return GF_OK;
}