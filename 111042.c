u64 gf_av1_leb128_read(GF_BitStream *bs, u8 *opt_Leb128Bytes) {
	u64 value = 0;
	u8 Leb128Bytes = 0, i = 0;
	for (i = 0; i < 8; i++) {
		u8 leb128_byte = gf_bs_read_u8(bs);
		value |= ( ((u64) (leb128_byte & 0x7f)) << (i * 7));
		Leb128Bytes += 1;
		if (!(leb128_byte & 0x80)) {
			break;
		}
	}

	if (opt_Leb128Bytes) {
		*opt_Leb128Bytes = Leb128Bytes;
	}
	return value;
}