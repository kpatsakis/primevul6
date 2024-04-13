static GFINLINE u8 gf_bs_load_byte(GF_BitStream *bs, Bool *is_eos)
{
	u8 res;
	if (bs->cache_read) {
		if (bs->cache_read_pos == bs->cache_read_size) {
			bs->cache_read_size = (u32) gf_fread(bs->cache_read, bs->cache_read_alloc, bs->stream);
			bs->cache_read_pos = 0;
			if (!bs->cache_read_size) {
				*is_eos = GF_TRUE;
				return 0;
			}
		}
		res = bs->cache_read[bs->cache_read_pos];
		bs->cache_read_pos++;
	} else {
		res = gf_fgetc(bs->stream);
	}
	return res;
}