u32 gf_isom_segment_get_track_fragment_count(GF_ISOFile *file, u32 moof_index)
{
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	GF_MovieFragmentBox *moof;
	if (!file) return 0;
	gf_list_count(file->TopBoxes);
	moof = gf_isom_get_moof(file, moof_index);
	return moof ? gf_list_count(moof->TrackList) : 0;
#endif
	return 0;
}