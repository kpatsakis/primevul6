GF_Err gf_isom_check_data_reference(GF_ISOFile *the_file, u32 trackNumber, u32 StreamDescriptionIndex)
{
	GF_Err e;
	u32 drefIndex;
	GF_TrackBox *trak;

	if (!StreamDescriptionIndex || !trackNumber) return GF_BAD_PARAM;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	e = Media_GetSampleDesc(trak->Media, StreamDescriptionIndex , NULL, &drefIndex);
	if (e) return e;
	if (!drefIndex) return GF_BAD_PARAM;
	return Media_CheckDataEntry(trak->Media, drefIndex);
}