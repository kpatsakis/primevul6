GF_Err gf_isom_get_chapter(GF_ISOFile *movie, u32 trackNumber, u32 Index, u64 *chapter_time, const char **name)
{
	GF_UserDataMap *map;
	GF_ChapterListBox *lst;
	GF_ChapterEntry *ce;
	GF_UserDataBox *udta;

	if (!movie || !movie->moov) return GF_BAD_PARAM;

	udta = NULL;
	if (trackNumber) {
		GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return GF_BAD_PARAM;
		udta = trak->udta;
	} else {
		udta = movie->moov->udta;
	}
	if (!udta) return GF_BAD_PARAM;
	map = udta_getEntry(movie->moov->udta, GF_ISOM_BOX_TYPE_CHPL, NULL);
	if (!map) return GF_BAD_PARAM;
	lst = (GF_ChapterListBox *)gf_list_get(map->boxes, 0);
	if (!lst) return GF_BAD_PARAM;

	ce = (GF_ChapterEntry *)gf_list_get(lst->list, Index-1);
	if (!ce) return GF_BAD_PARAM;
	if (chapter_time) {
		*chapter_time = ce->start_time;
		*chapter_time /= 10000L;
	}
	if (name) *name = ce->name;
	return GF_OK;
}