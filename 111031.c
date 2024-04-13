u32 gf_m4a_get_profile(GF_M4ADecSpecInfo *cfg)
{
	switch (cfg->base_object_type) {
	case 2: /*AAC LC*/
		if (cfg->nb_chan <= 2)
			return (cfg->base_sr <= 24000) ? 0x28 : 0x29; /*LC@L1 or LC@L2*/
		if (cfg->nb_chan <= 5)
			return (cfg->base_sr <= 48000) ? 0x2A : 0x2B; /*LC@L4 or LC@L5*/
		return (cfg->base_sr <= 48000) ? 0x50 : 0x51; /*LC@L4 or LC@L5*/
	case 5: /*HE-AAC - SBR*/
		if (cfg->nb_chan <= 2)
			return (cfg->base_sr <= 24000) ? 0x2C : 0x2D; /*HE@L2 or HE@L3*/
		if (cfg->nb_chan <= 5)
			return (cfg->base_sr <= 48000) ? 0x2E : 0x2F; /*HE@L4 or HE@L5*/
		return (cfg->base_sr <= 48000) ? 0x52 : 0x53; /*HE@L6 or HE@L7*/
	case 29: /*HE-AACv2 - SBR+PS*/
		if (cfg->nb_chan <= 2)
			return (cfg->base_sr <= 24000) ? 0x30 : 0x31; /*HE-AACv2@L2 or HE-AACv2@L3*/
		if (cfg->nb_chan <= 5)
			return (cfg->base_sr <= 48000) ? 0x32 : 0x33; /*HE-AACv2@L4 or HE-AACv2@L5*/
		return (cfg->base_sr <= 48000) ? 0x54 : 0x55; /*HE-AACv2@L6 or HE-AACv2@L7*/
	/*default to HQ*/
	default:
		if (cfg->nb_chan <= 2) return (cfg->base_sr < 24000) ? 0x0E : 0x0F; /*HQ@L1 or HQ@L2*/
		return 0x10; /*HQ@L3*/
	}
}