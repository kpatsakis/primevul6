static Bool AC3_FindSyncCodeBS(GF_BitStream *bs)
{
	u8 b1;
	u64 pos = gf_bs_get_position(bs);
	u64 end = gf_bs_get_size(bs);

	pos += 1;
	b1 = gf_bs_read_u8(bs);
	while (pos + 1 <= end) {
		u8 b2 = gf_bs_read_u8(bs);
		if ((b1 == 0x0b) && (b2 == 0x77)) {
			gf_bs_seek(bs, pos - 1);
			return GF_TRUE;
		}
		pos++;
		b1 = b2;
	}
	return GF_FALSE;
}