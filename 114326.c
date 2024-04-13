u32 gf_isom_get_udta_count(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackBox *trak;
	GF_UserDataBox *udta;
	if (!movie || !movie->moov) return 0;

	if (trackNumber) {
		trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return 0;
		udta = trak->udta;
	} else {
		udta = movie->moov->udta;
	}
	if (udta) return gf_list_count(udta->recordList);
	return 0;
}