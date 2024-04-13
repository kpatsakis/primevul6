static s32 bs_handle_nbits_overflow(GF_BitStream* bs, s32 nBits, s32 max_shift)
{
	if (nBits > max_shift) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[BS] Attempt to write %d bits, when max is %d\n", nBits, max_shift));
	}
	while (nBits > max_shift) {
		gf_bs_write_long_int(bs, 0, max_shift);
		nBits -= max_shift;
	}

	return nBits;
}