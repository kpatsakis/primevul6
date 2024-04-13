u32 gf_media_nalu_is_start_code(GF_BitStream *bs)
{
	u8 s1, s2, s3, s4;
	Bool is_sc = 0;
	u64 pos = gf_bs_get_position(bs);
	s1 = gf_bs_read_int(bs, 8);
	s2 = gf_bs_read_int(bs, 8);
	if (!s1 && !s2) {
		s3 = gf_bs_read_int(bs, 8);
		if (s3 == 0x01) is_sc = 3;
		else if (!s3) {
			s4 = gf_bs_read_int(bs, 8);
			if (s4 == 0x01) is_sc = 4;
		}
	}
	gf_bs_seek(bs, pos + is_sc);
	return is_sc;
}