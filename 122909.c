u8 gf_bs_align(GF_BitStream *bs)
{
	u8 res = 8 - bs->nbBits;
	if ( (bs->bsmode == GF_BITSTREAM_READ) || (bs->bsmode == GF_BITSTREAM_FILE_READ)) {
		if (res > 0) {
			gf_bs_read_int(bs, res);
		}
		return res;
	}
	if (bs->nbBits > 0) {
		gf_bs_write_int (bs, 0, res);
		return res;
	}
	return 0;
}