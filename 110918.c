u16 gf_mp3_window_size(u32 hdr)
{
	u8 version = gf_mp3_version(hdr);
	u8 layer = gf_mp3_layer(hdr);

	if (layer == 3) {
		if (version == 3) return 1152;
		return 576;
	}
	if (layer == 2) return 1152;
	return 384;
}