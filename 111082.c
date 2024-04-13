void gf_bs_write_ue(GF_BitStream *bs, u32 num) {
	s32 length = 1;
	s32 temp = ++num;

	while (temp != 1) {
		temp >>= 1;
		length += 2;
	}

	gf_bs_write_int(bs, 0, length >> 1);
	gf_bs_write_int(bs, num, (length + 1) >> 1);
}