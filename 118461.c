Bool gf_m2ts_crc32_check(u8 *data, u32 len)
{
	u32 crc = gf_crc_32(data, len);
	u32 crc_val = GF_4CC((u8) data[len], (u8) data[len+1], (u8) data[len+2], (u8) data[len+3]);
	return (crc==crc_val) ? GF_TRUE : GF_FALSE;
}