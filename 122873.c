u8 gf_bs_bits_available(GF_BitStream *bs)
{
	if (bs->size > bs->position) return 8;
	if (bs->nbBits < 8) return (8-bs->nbBits);
	return 0;
}