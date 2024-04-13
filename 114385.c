GF_Err gf_isom_get_udta_type(GF_ISOFile *movie, u32 trackNumber, u32 udta_idx, u32 *UserDataType, bin128 *UUID)
{
	GF_TrackBox *trak;
	GF_UserDataBox *udta;
	GF_UserDataMap *map;
	if (!movie || !movie->moov || !udta_idx) return GF_BAD_PARAM;

	if (trackNumber) {
		trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return GF_OK;
		udta = trak->udta;
	} else {
		udta = movie->moov->udta;
	}
	if (!udta) return GF_BAD_PARAM;
	if (udta_idx>gf_list_count(udta->recordList)) return GF_BAD_PARAM;
	map = (GF_UserDataMap*)gf_list_get(udta->recordList, udta_idx - 1);
	if (UserDataType) *UserDataType = map->boxType;
	if (UUID) memcpy(*UUID, map->uuid, 16);
	return GF_OK;
}