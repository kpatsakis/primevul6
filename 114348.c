GF_Err gf_isom_get_reference_ID(GF_ISOFile *movie, u32 trackNumber, u32 referenceType, u32 referenceIndex, GF_ISOTrackID *refTrackID)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_TrackReferenceTypeBox *dpnd;
	trak = gf_isom_get_track_from_file(movie, trackNumber);

	*refTrackID = 0;
	if (!trak || !trak->References || !referenceIndex) return GF_BAD_PARAM;

	dpnd = NULL;
	e = Track_FindRef(trak, referenceType, &dpnd);
	if (e) return e;
	if (!dpnd) return GF_BAD_PARAM;

	if (referenceIndex > dpnd->trackIDCount) return GF_BAD_PARAM;

	*refTrackID = dpnd->trackIDs[referenceIndex-1];

	return GF_OK;
}