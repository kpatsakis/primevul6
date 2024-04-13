u32 gf_ac3_get_channels(u32 acmod)
{
	u32 nb_ch;
	nb_ch = ac3_mod_to_chans[acmod];
	return nb_ch;
}