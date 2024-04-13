GF_Err gf_isom_get_copyright(GF_ISOFile *mov, u32 Index, const char **threeCharCode, const char **notice)
{
	GF_UserDataMap *map;
	GF_CopyrightBox *cprt;

	if (!mov || !mov->moov || !Index) return GF_BAD_PARAM;

	if (!mov->moov->udta) return GF_OK;
	map = udta_getEntry(mov->moov->udta, GF_ISOM_BOX_TYPE_CPRT, NULL);
	if (!map) return GF_OK;

	if (Index > gf_list_count(map->boxes)) return GF_BAD_PARAM;

	cprt = (GF_CopyrightBox*)gf_list_get(map->boxes, Index-1);
	(*threeCharCode) = cprt->packedLanguageCode;
	(*notice) = cprt->notice;
	return GF_OK;
}