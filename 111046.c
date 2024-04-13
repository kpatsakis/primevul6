s32 M4V_LoadObject(GF_M4VParser *m4v)
{
	u32 v, bpos, found;
	char m4v_cache[M4V_CACHE_SIZE];
	u64 end, cache_start, load_size;
	if (!m4v) return 0;
	if (m4v->force_next_obj_type) {
		m4v->current_object_type = m4v->force_next_obj_type - 1;
		m4v->force_next_obj_type = 0;
		return (s32)m4v->current_object_type;
	}

	bpos = 0;
	found = 0;
	load_size = 0;
	end = 0;
	cache_start = 0;
	v = 0xffffffff;
	while (!end) {
		/*refill cache*/
		if (bpos == (u32)load_size) {
			if (!gf_bs_available(m4v->bs)) break;
			load_size = gf_bs_available(m4v->bs);
			if (load_size > M4V_CACHE_SIZE) load_size = M4V_CACHE_SIZE;
			bpos = 0;
			cache_start = gf_bs_get_position(m4v->bs);
			gf_bs_read_data(m4v->bs, m4v_cache, (u32)load_size);
		}
		v = ((v << 8) & 0xFFFFFF00) | ((u8)m4v_cache[bpos]);
		bpos++;
		if ((v & 0xFFFFFF00) == 0x00000100) {
			end = cache_start + bpos - 4;
			found = 1;
			break;
		}
	}
	if (!found) return -1;
	m4v->current_object_start = end;
	gf_bs_seek(m4v->bs, end + 3);
	m4v->current_object_type = gf_bs_read_u8(m4v->bs);
	return (s32)m4v->current_object_type;
}