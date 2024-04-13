u32 gf_isom_segment_get_track_fragment_decode_time(GF_ISOFile *file, u32 moof_index, u32 traf_index, u64 *decode_time)
{
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	GF_MovieFragmentBox *moof;
	GF_TrackFragmentBox *traf;
	if (!file) return 0;
	gf_list_count(file->TopBoxes);
	moof = gf_isom_get_moof(file, moof_index);
	traf = moof ? (GF_TrackFragmentBox*)gf_list_get(moof->TrackList, traf_index-1) : NULL;
	if (!traf) return 0;
	if (decode_time) {
		*decode_time = traf->tfdt ? traf->tfdt->baseMediaDecodeTime : 0;
	}
	return traf->tfhd->trackID;
#endif
	return 0;
}