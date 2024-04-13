const char *gf_avc_get_profile_name(u8 video_prof)
{
	switch (video_prof) {
	case 0x42:
		return "Baseline";
	case 0x4D:
		return "Main";
	case 0x53:
		return "Scalable Baseline";
	case 0x56:
		return "Scalable High";
	case 0x58:
		return "Extended";
	case 0x64:
		return "High";
	case 0x6E:
		return "High 10";
	case 0x7A:
		return "High 4:2:2";
	case 0x90:
	case 0xF4:
		return "High 4:4:4";
	default:
		return "Unknown";
	}
}