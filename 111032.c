const char *gf_mp3_version_name(u32 hdr)
{
	u32 v = gf_mp3_version(hdr);
	switch (v) {
	case 0:
		return "MPEG-2.5";
	case 1:
		return "Reserved";
	case 2:
		return "MPEG-2";
	case 3:
		return "MPEG-1";
	default:
		return "Unknown";
	}
}