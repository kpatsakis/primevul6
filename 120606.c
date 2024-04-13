
GF_Err udta_AddBox(GF_Box *s, GF_Box *a)
{
	GF_Err e;
	u32 box_type;
	GF_UserDataMap *map;
	GF_UserDataBox *ptr = (GF_UserDataBox *)s;
	if (!ptr) return GF_BAD_PARAM;
	if (!a) return GF_OK;

	/* for unknown udta boxes, we reference them by their original box type */
	box_type = a->type;
	if (box_type == GF_ISOM_BOX_TYPE_UNKNOWN) {
		GF_UnknownBox* unkn = (GF_UnknownBox *)a;
		if (unkn)
			box_type = unkn->original_4cc;
	}

	map = udta_getEntry(ptr, box_type, (a->type==GF_ISOM_BOX_TYPE_UUID) ? & ((GF_UUIDBox *)a)->uuid : NULL);
	if (map == NULL) {
		map = (GF_UserDataMap *) gf_malloc(sizeof(GF_UserDataMap));
		if (map == NULL) return GF_OUT_OF_MEM;
		memset(map, 0, sizeof(GF_UserDataMap));

		map->boxType = box_type;
		if (a->type == GF_ISOM_BOX_TYPE_UUID)
			memcpy(map->uuid, ((GF_UUIDBox *)a)->uuid, 16);
		map->other_boxes = gf_list_new();
		if (!map->other_boxes) {
			gf_free(map);
			return GF_OUT_OF_MEM;
		}
		e = gf_list_add(ptr->recordList, map);
		if (e) return e;
	}
	return gf_list_add(map->other_boxes, a);