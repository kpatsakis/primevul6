GF_Err gf_isom_get_track_kind(GF_ISOFile *the_file, u32 trackNumber, u32 index, char **scheme, char **value)
{
	GF_Err e;
	GF_UserDataBox *udta;
	GF_UserDataMap *map;
	GF_KindBox *kindBox;
	if (!scheme || !value) {
		return GF_BAD_PARAM;
	}
	*scheme = NULL;
	*value = NULL;

	if (trackNumber) {
		GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
		if (!trak) return GF_BAD_PARAM;
		if (!trak->udta) {
			e = trak_on_child_box((GF_Box*)trak, gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_UDTA), GF_FALSE);
			if (e) return e;
		}
		udta = trak->udta;
	} else {
		return GF_BAD_PARAM;
	}
	map = udta_getEntry(udta, GF_ISOM_BOX_TYPE_KIND, NULL);
	if (!map) return GF_BAD_PARAM;

	kindBox = (GF_KindBox *)gf_list_get(map->boxes, index);
	if (!kindBox) return GF_BAD_PARAM;

	*scheme = gf_strdup(kindBox->schemeURI);
	if (kindBox->value) {
		*value = gf_strdup(kindBox->value);
	}
	return GF_OK;
}