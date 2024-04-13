u64 gf_av1_leb128_write(GF_BitStream *bs, u64 value)
{
	u32 i, leb_size = gf_av1_leb128_size(value);
	for (i = 0; i < leb_size; ++i) {
		u8 byte = value & 0x7f;
		value >>= 7;
		if (value != 0) byte |= 0x80; //more bytes follow
		gf_bs_write_u8(bs, byte);
	}

	return leb_size;
}