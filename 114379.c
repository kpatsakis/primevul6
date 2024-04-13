GF_Err gf_isom_set_sample_padding(GF_ISOFile *the_file, u32 trackNumber, u32 padding_bytes)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	trak->padding_bytes = padding_bytes;
	return GF_OK;

}