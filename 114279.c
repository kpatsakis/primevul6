u32 gf_isom_get_user_data_count(GF_ISOFile *movie, u32 trackNumber, u32 UserDataType, bin128 UUID)
{
	GF_UserDataMap *map;
	GF_TrackBox *trak;
	GF_UserDataBox *udta;
	bin128 t;
	u32 i, count;

	if (!movie || !movie->moov) return 0;

	if (UserDataType == GF_ISOM_BOX_TYPE_UUID) UserDataType = 0;
	memset(t, 1, 16);

	if (trackNumber) {
		trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return 0;
		udta = trak->udta;
	} else {
		udta = movie->moov->udta;
	}
	if (!udta) return 0;

	i=0;
	while ((map = (GF_UserDataMap*)gf_list_enum(udta->recordList, &i))) {
		count = gf_list_count(map->boxes);

		if ((map->boxType == GF_ISOM_BOX_TYPE_UUID) && !memcmp(map->uuid, UUID, 16)) return count;
		else if (map->boxType == UserDataType) return count;
	}
	return 0;
}