Bool gf_bs_is_align(GF_BitStream *bs)
{
	switch (bs->bsmode) {
	case GF_BITSTREAM_READ:
	case GF_BITSTREAM_FILE_READ:
		return ( (8 == bs->nbBits) ? GF_TRUE : GF_FALSE);
	default:
		return !bs->nbBits;
	}
}