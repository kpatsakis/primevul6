GF_Err gf_isom_get_sample_padding_bits(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber, u8 *NbBits)
{
	GF_TrackBox *trak;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;


	//Padding info
	return stbl_GetPaddingBits(trak->Media->information->sampleTable->PaddingBits,
	                           sampleNumber, NbBits);

}