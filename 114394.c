u32 gf_isom_get_track_kind_count(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_UserDataBox *udta;
	GF_UserDataMap *map;
	if (trackNumber) {
		GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
		if (!trak) return 0;
		if (!trak->udta) {
			return 0;
		}
		udta = trak->udta;
	} else {
		return 0;
	}
	map = udta_getEntry(udta, GF_ISOM_BOX_TYPE_KIND, NULL);
	if (!map) return 0;

	return gf_list_count(map->boxes);
}