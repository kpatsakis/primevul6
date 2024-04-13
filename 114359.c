GF_ISOSample *gf_isom_get_sample(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber, u32 *sampleDescriptionIndex)
{
	return gf_isom_get_sample_ex(the_file, trackNumber, sampleNumber, sampleDescriptionIndex, NULL, NULL);
}