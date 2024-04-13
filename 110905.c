static Bool vp9_frame_sync_code(GF_BitStream *bs)
{
	u8 val = gf_bs_read_int_log(bs, 8, "syncbyte1");
	if (val != 0x49)
		return GF_FALSE;

	val = gf_bs_read_int_log(bs, 8, "syncbyte2");
	if (val != 0x83)
		return GF_FALSE;

	val = gf_bs_read_int_log(bs, 8, "syncbyte3");
	if (val != 0x42)
		return GF_FALSE;

	return GF_TRUE;
}