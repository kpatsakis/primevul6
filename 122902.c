Double gf_bs_read_double(GF_BitStream *bs)
{
	char buf [8] = "\0\0\0\0\0\0\0";
	s32 i;
	for (i = 0; i < 64; i++)
		buf[7-i/8] |= gf_bs_read_bit(bs) << (7 - i%8);
	return (* (Double *) buf);
}