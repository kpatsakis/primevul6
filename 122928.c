Float gf_bs_read_float(GF_BitStream *bs)
{
	char buf [4] = "\0\0\0";
#ifdef NO_OPTS
	s32 i;
	for (i = 0; i < 32; i++)
		buf[3-i/8] |= gf_bs_read_bit(bs) << (7 - i%8);
#else
	buf[3] = gf_bs_read_int(bs, 8);
	buf[2] = gf_bs_read_int(bs, 8);
	buf[1] = gf_bs_read_int(bs, 8);
	buf[0] = gf_bs_read_int(bs, 8);
#endif
	return (* (Float *) buf);
}