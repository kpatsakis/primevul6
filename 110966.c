GF_M4VParser *gf_m4v_parser_new(u8 *data, u64 data_size, Bool mpeg12video)
{
	GF_M4VParser *tmp;
	if (!data || !data_size) return NULL;
	GF_SAFEALLOC(tmp, GF_M4VParser);
	if (!tmp) return NULL;
	tmp->bs = gf_bs_new(data, data_size, GF_BITSTREAM_READ);
	tmp->mpeg12 = mpeg12video;
	return tmp;
}