void gf_bs_rewind_bits(GF_BitStream *bs, u64 nbBits)
{
	u64 nbBytes;
	if (bs->bsmode != GF_BITSTREAM_READ) return;

	nbBits -= (bs->nbBits);
	nbBytes = (nbBits+8)>>3;
	nbBits = nbBytes*8 - nbBits;
	gf_bs_align(bs);
	assert(bs->position >= nbBytes);
	bs->position -= nbBytes + 1;
	gf_bs_read_int(bs, (u32)nbBits);
	return;
}