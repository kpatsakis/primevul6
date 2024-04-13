Bool gf_media_probe_ivf(GF_BitStream *bs)
{
	u32 dw = 0;
	if (gf_bs_available(bs) < IVF_FILE_HEADER_SIZE) return GF_FALSE;

	dw = gf_bs_peek_bits(bs, 32, 0);
	if (dw != GF_4CC('D', 'K', 'I', 'F')) {
		return GF_FALSE;
	}
	return GF_TRUE;
}