const char *gf_m4a_get_profile_name(u8 audio_pl)
{
	u32 i, count = GF_ARRAY_LENGTH(M4AProfiles);
	for (i=0; i<count; i++) {
		if ((u32) audio_pl==M4AProfiles[i].value)
			return M4AProfiles[i].name;
	}
	return "ISO Reserved / User Private";
}