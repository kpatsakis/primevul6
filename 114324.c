const GF_ISOTrackID *gf_isom_enum_track_references(GF_ISOFile *movie, u32 trackNumber, u32 idx, u32 *referenceType, u32 *referenceCount)
{
	GF_TrackBox *trak;
	GF_TrackReferenceTypeBox *dpnd;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return NULL;
	if (!trak->References) return NULL;
	dpnd = gf_list_get(trak->References->child_boxes, idx);
	if (!dpnd) return NULL;
	*referenceType = dpnd->reference_type;
	*referenceCount = dpnd->trackIDCount;
	return dpnd->trackIDs;
}