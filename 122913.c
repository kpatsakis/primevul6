void gf_bs_write_double (GF_BitStream *bs, Double value)
{
	u32 i;
	union
	{	Double d;
		char sz [8];
	} double_value;
	double_value.d = value;
	for (i = 0; i < 64; i++) {
		BS_WriteBit(bs, (double_value.sz [7 - i / 8] & 1 << (7 - i % 8)) != 0);
	}
}