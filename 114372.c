GF_Err gf_isom_get_watermark(GF_ISOFile *mov, bin128 UUID, u8** data, u32* length)
{
	GF_UserDataMap *map;
	GF_UnknownUUIDBox *wm;

	if (!mov) return GF_BAD_PARAM;
	if (!mov->moov || !mov->moov->udta) return GF_NOT_SUPPORTED;

	map = udta_getEntry(mov->moov->udta, GF_ISOM_BOX_TYPE_UUID, (bin128 *) & UUID);
	if (!map) return GF_NOT_SUPPORTED;

	wm = (GF_UnknownUUIDBox*)gf_list_get(map->boxes, 0);
	if (!wm) return GF_NOT_SUPPORTED;

	*data = (u8 *) gf_malloc(sizeof(char)*wm->dataSize);
	if (! *data) return GF_OUT_OF_MEM;
	memcpy(*data, wm->data, wm->dataSize);
	*length = wm->dataSize;
	return GF_OK;
}