GF_Err gf_isom_get_media_time(GF_ISOFile *the_file, u32 trackNumber, u32 movieTime, u64 *MediaTime)
{
	GF_TrackBox *trak;
	u8 useEdit;
	s64 SegmentStartTime, mediaOffset;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !MediaTime) return GF_BAD_PARAM;

	SegmentStartTime = 0;
	return GetMediaTime(trak, GF_FALSE, movieTime, MediaTime, &SegmentStartTime, &mediaOffset, &useEdit, NULL);
}