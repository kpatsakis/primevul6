u32 gf_isom_get_copyright_count(GF_ISOFile *mov)
{
	GF_UserDataMap *map;
	if (!mov || !mov->moov || !mov->moov->udta) return 0;
	map = udta_getEntry(mov->moov->udta, GF_ISOM_BOX_TYPE_CPRT, NULL);
	if (!map) return 0;
	return gf_list_count(map->boxes);
}