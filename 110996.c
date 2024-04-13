u8 gf_mp3_version(u32 hdr)
{
	return ((hdr >> 19) & 0x3);
}