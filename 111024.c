Bool gf_avc_is_rext_profile(u8 profile_idc)
{
	switch (profile_idc) {
	case 100:
	case 110:
	case 122:
	case 244:
	case 44:
	case 83:
	case 86:
	case 118:
	case 128:
	case 138:
	case 139:
	case 134:
	case 135:
		return GF_TRUE;
	default:
		return GF_FALSE;
	}
}