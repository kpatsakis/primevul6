static u32 gf_media_nalu_locate_start_code_bs(GF_BitStream *bs, Bool locate_trailing)
{
	u32 v, bpos, nb_cons_zeros = 0;
	char avc_cache[AVC_CACHE_SIZE];
	u64 end, cache_start, load_size;
	u64 start = gf_bs_get_position(bs);
	if (start < 3) return 0;

	load_size = 0;
	bpos = 0;
	cache_start = 0;
	end = 0;
	v = 0xffffffff;
	while (!end) {
		/*refill cache*/
		if (bpos == (u32)load_size) {
			if (!gf_bs_available(bs)) break;
			load_size = gf_bs_available(bs);
			if (load_size > AVC_CACHE_SIZE) load_size = AVC_CACHE_SIZE;
			bpos = 0;
			cache_start = gf_bs_get_position(bs);
			gf_bs_read_data(bs, avc_cache, (u32)load_size);
		}
		v = ( (v<<8) & 0xFFFFFF00) | ((u32) avc_cache[bpos]);
		bpos++;

		if (locate_trailing) {
			if ((v & 0x000000FF) == 0) nb_cons_zeros++;
			else nb_cons_zeros = 0;
		}

		if (v == 0x00000001) end = cache_start + bpos - 4;
		else if ((v & 0x00FFFFFF) == 0x00000001) end = cache_start + bpos - 3;
	}

	gf_bs_seek(bs, start);
	if (!end) end = gf_bs_get_size(bs);
	if (locate_trailing) {
		if (nb_cons_zeros >= 3)
			return (u32)(end - start - nb_cons_zeros);
	}
	return (u32)(end - start);
}