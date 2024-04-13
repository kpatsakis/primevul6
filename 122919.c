u32 gf_bs_get_bit_offset(GF_BitStream *bs)
{
	if (bs->bsmode==GF_BITSTREAM_READ) return (u32) ( (bs->position - 1) * 8 + bs->nbBits);
	return (u32) ( (bs->position ) * 8 + bs->nbBits);
}