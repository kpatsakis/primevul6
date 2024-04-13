u8 gf_mp3_layer(u32 hdr)
{
	return 4 - (((hdr >> 17) & 0x3));
}