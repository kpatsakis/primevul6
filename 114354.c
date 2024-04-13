u32 gf_isom_get_chapter_count(GF_ISOFile *movie, u32 trackNumber)
{
	GF_UserDataMap *map;
	GF_ChapterListBox *lst;
	GF_UserDataBox *udta;

	if (!movie || !movie->moov) return 0;

	udta = NULL;
	if (trackNumber) {
		GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return 0;
		udta = trak->udta;
	} else {
		udta = movie->moov->udta;
	}
	if (!udta) return 0;
	map = udta_getEntry(udta, GF_ISOM_BOX_TYPE_CHPL, NULL);
	if (!map) return 0;
	lst = (GF_ChapterListBox *)gf_list_get(map->boxes, 0);
	if (!lst) return 0;
	return gf_list_count(lst->list);
}