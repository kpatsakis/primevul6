const u32 *gf_isom_get_track_switch_parameter(GF_ISOFile *movie, u32 trackNumber, u32 group_index, u32 *switchGroupID, u32 *criteriaListSize)
{
	GF_TrackBox *trak;
	GF_UserDataMap *map;
	GF_TrackSelectionBox *tsel;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!group_index || !trak || !trak->udta) return NULL;

	map = udta_getEntry(trak->udta, GF_ISOM_BOX_TYPE_TSEL, NULL);
	if (!map) return NULL;
	tsel = (GF_TrackSelectionBox*)gf_list_get(map->boxes, group_index-1);
	if (!tsel) return NULL;
	
	*switchGroupID = tsel->switchGroup;
	*criteriaListSize = tsel->attributeListCount;
	return (const u32 *) tsel->attributeList;
}