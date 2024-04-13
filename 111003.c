const char *gf_avc_hevc_get_chroma_format_name(u8 chroma_format)
{
	switch (chroma_format) {
	case 1:
		return "YUV 4:2:0";
	case 2:
		return "YUV 4:2:2";
	case 3:
		return "YUV 4:4:4";
	default:
		return "Unknown";
	}
}