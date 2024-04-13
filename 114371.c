GF_Err gf_isom_open_progressive(const char *fileName, u64 start_range, u64 end_range, Bool enable_frag_bounds, GF_ISOFile **the_file, u64 *BytesMissing)
{
	return gf_isom_open_progressive_ex(fileName, start_range, end_range, enable_frag_bounds, the_file, BytesMissing, NULL);
}