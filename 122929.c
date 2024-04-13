void gf_bs_write_long_int(GF_BitStream *bs, s64 _value, s32 nBits)
{
	s32 max_shift = sizeof(s64) * 8;
	if (!nBits) return;
	nBits = bs_handle_nbits_overflow(bs, nBits, max_shift);

	//cf note in gf_bs_write_int
	u64 value = (u64) _value;
	value <<= max_shift - nBits;
	while (--nBits >= 0) {
		BS_WriteBit (bs, ((s64)value) < 0);
		value <<= 1;
	}
}