u32 gf_bs_read_int(GF_BitStream *bs, u32 nBits)
{
	u32 ret;
	bs->total_bits_read+= nBits;

#ifndef NO_OPTS
	if (nBits + bs->nbBits <= 8) {
		bs->nbBits += nBits;
		ret = (bs->current >> (8 - bs->nbBits) ) & bits_mask[nBits];
		return ret;
	}
#endif
	ret = 0;
	while (nBits-- > 0) {
		ret <<= 1;
		ret |= gf_bs_read_bit(bs);
	}
	return ret;
}