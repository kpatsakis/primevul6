
GF_Err reftype_AddRefTrack(GF_TrackReferenceTypeBox *ref, u32 trackID, u16 *outRefIndex)
{
	u32 i;
	if (!ref || !trackID) return GF_BAD_PARAM;

	if (outRefIndex) *outRefIndex = 0;
	//don't add a dep if already here !!
	for (i = 0; i < ref->trackIDCount; i++) {
		if (ref->trackIDs[i] == trackID) {
			if (outRefIndex) *outRefIndex = i+1;
			return GF_OK;
		}
	}

	ref->trackIDs = (u32 *) gf_realloc(ref->trackIDs, (ref->trackIDCount + 1) * sizeof(u32) );
	if (!ref->trackIDs) return GF_OUT_OF_MEM;
	ref->trackIDs[ref->trackIDCount] = trackID;
	ref->trackIDCount++;
	if (outRefIndex) *outRefIndex = ref->trackIDCount;
	return GF_OK;