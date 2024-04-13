static u32 av1_decode_subexp(GF_BitStream *bs, s32 numSyms)
{
	s32 i = 0;
	s32 mk = 0;
	s32 k = 3;
	while (1) {
		s32 b2 = i ? k + i - 1 : k;
		s32 a = 1 << b2;
		if (numSyms <= mk + 3 * a) {
			s32 subexp_final_bits = av1_read_ns(bs, numSyms - mk, NULL);
			return subexp_final_bits + mk;
		}
		else {
			s32 subexp_more_bits = gf_bs_read_int(bs, 1);
			if (subexp_more_bits) {
				i++;
				mk += a;
			}
			else {
				s32 subexp_bits = gf_bs_read_int(bs, b2);
				return subexp_bits + mk;
			}
		}
	}
}