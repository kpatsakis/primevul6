GF_Err gf_isom_get_data_reference(GF_ISOFile *the_file, u32 trackNumber, u32 StreamDescriptionIndex, const char **outURL, const char **outURN)
{
	GF_TrackBox *trak;
	GF_DataEntryURLBox *url=NULL;
	GF_DataEntryURNBox *urn;
	u32 drefIndex;
	GF_Err e;

	*outURL = *outURN = NULL;

	if (!StreamDescriptionIndex || !trackNumber) return GF_BAD_PARAM;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	e = Media_GetSampleDesc(trak->Media, StreamDescriptionIndex , NULL, &drefIndex);
	if (e) return e;
	if (!drefIndex) return GF_BAD_PARAM;

	if (trak->Media
		&& trak->Media->information
		&& trak->Media->information->dataInformation
		&& trak->Media->information->dataInformation->dref
	) {
		url = (GF_DataEntryURLBox*)gf_list_get(trak->Media->information->dataInformation->dref->child_boxes, drefIndex - 1);
	}
	if (!url) return GF_ISOM_INVALID_FILE;

	if (url->type == GF_ISOM_BOX_TYPE_URL) {
		*outURL = url->location;
		*outURN = NULL;
	} else if (url->type == GF_ISOM_BOX_TYPE_URN) {
		urn = (GF_DataEntryURNBox *) url;
		*outURN = urn->nameURN;
		*outURL = urn->location;
	} else {
		*outURN = NULL;
		*outURL = NULL;
	}
	return GF_OK;
}