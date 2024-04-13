u64 gf_bs_read_long_int(GF_BitStream *bs, u32 nBits)
{
	u64 ret = 0;
	if (nBits>64) {
		u32 skip = nBits-64;
		if (gf_bs_available(bs) * 8 < nBits-8) {
			if (bs->EndOfStream) bs->EndOfStream(bs->par);
			bs->position = bs->size;
			if (!bs->overflow_state) bs->overflow_state = 1;
			return 0;
		}
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("Reading %d bits but max should be 64, skipping %d most significants bits\n", nBits, nBits-64));
		//avoid recursion
		while (skip) {
			if (skip>32) {
				gf_bs_read_int(bs, 32);
				skip-=32;
			} else {
				gf_bs_read_int(bs, skip);
				skip=0;
			}
		}
		ret = gf_bs_read_long_int(bs, 64);
	} else {
		while (nBits-- > 0) {
			ret <<= 1;
			ret |= gf_bs_read_bit(bs);
		}
	}
	return ret;
}