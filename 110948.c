u32 gf_m4a_get_channel_cfg(u32 nb_chan)
{
	u32 i, count = sizeof(GF_M4ANumChannels) / sizeof(u32);
	for (i = 0; i < count; i++) {
		if (GF_M4ANumChannels[i] == nb_chan) return i + 1;
	}
	return 0;
}