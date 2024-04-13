void gf_bs_write_int(GF_BitStream *bs, s32 _value, s32 nBits)
{
	u32 value, nb_shift;
	s32 max_shift = sizeof(s32) * 8;
	if (!nBits) return;
	nBits = bs_handle_nbits_overflow(bs, nBits, max_shift);
	//move to unsigned to avoid sanitizer warnings when we pass a value not codable on the given number of bits
	//we do this when setting bit fields to all 1's
	value = (u32) _value;
	nb_shift = max_shift - nBits;
	if (nb_shift)
		value <<= nb_shift;

	while (--nBits >= 0) {
		//but check value as signed
		BS_WriteBit (bs, ((s32)value) < 0);
		value <<= 1;
	}
}