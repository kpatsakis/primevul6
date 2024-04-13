u32 gf_isom_probe_data(const u8*inBuf, u32 inSize)
{
	u32 type;
	if (inSize < 8) return 0;
	type = GF_4CC(inBuf[4], inBuf[5], inBuf[6], inBuf[7]);
	return gf_isom_probe_type(type);
}