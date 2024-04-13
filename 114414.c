GF_ISOSample *gf_isom_get_sample_info(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber, u32 *sampleDescriptionIndex, u64 *data_offset)
{
	return gf_isom_get_sample_info_ex(the_file, trackNumber, sampleNumber, sampleDescriptionIndex, data_offset, NULL);
}