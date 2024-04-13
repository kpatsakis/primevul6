GF_Err gf_bs_transfer(GF_BitStream *dst, GF_BitStream *src, Bool keep_src)
{
	u8 *data;
	u32 data_len, written;

	data = NULL;
	data_len = 0;
	gf_bs_get_content(src, &data, &data_len);
	if (!data || !data_len)
	{
		if (data) {
			if (keep_src) {
				src->original = data;
				src->size = data_len;
			} else {
				gf_free(data);
			}
			return GF_IO_ERR;
		}
		return GF_OK;
	}
	written = gf_bs_write_data(dst, data, data_len);
	if (keep_src) {
		src->original = data;
		src->size = data_len;
	} else {
		gf_free(data);
	}
	if (written<data_len) return GF_IO_ERR;
	return GF_OK;
}