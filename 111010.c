u32 gf_ac3_get_bitrate(u32 brcode)
{
	return ac3_sizecod_to_bitrate[brcode];
}