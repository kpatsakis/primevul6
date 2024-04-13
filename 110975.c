static u8 m4v_get_sar_idx(u32 w, u32 h)
{
	u32 i;
	for (i = 0; i < 6; i++) {
		if ((m4v_sar[i].w == w) && (m4v_sar[i].h == h)) return i;
	}
	return 0xF;
}