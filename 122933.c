void gf_bs_write_float(GF_BitStream *bs, Float value)
{
	u32 i;
	union
	{	float f;
		char sz [4];
	} float_value;
	float_value.f = value;

	for (i = 0; i < 32; i++)
		BS_WriteBit(bs, (float_value.sz [3 - i / 8] & 1 << (7 - i % 8)) != 0);

}