u8 gf_mp3_num_channels(u32 hdr)
{
	if (((hdr >> 6) & 0x3) == 3) return 1;
	return 2;
}