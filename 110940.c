static u8 avc_hevc_get_sar_idx(u32 w, u32 h)
{
	u32 i, count = GF_ARRAY_LENGTH(avc_hevc_sar);
	for (i = 0; i < count; i++) {
		if ((avc_hevc_sar[i].w == w) && (avc_hevc_sar[i].h == h))
			return i;
	}
	return 0xFF;
}