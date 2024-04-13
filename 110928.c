GF_Err gf_m4a_write_config_bs(GF_BitStream *bs, GF_M4ADecSpecInfo *cfg)
{
	if (!cfg->base_sr_index) {
		if (!cfg->base_sr) return GF_BAD_PARAM;
		while (GF_M4ASampleRates[cfg->base_sr_index]) {
			if (GF_M4ASampleRates[cfg->base_sr_index] == cfg->base_sr)
				break;
			cfg->base_sr_index++;
		}
	}
	if (cfg->sbr_sr && !cfg->sbr_sr_index) {
		while (GF_M4ASampleRates[cfg->sbr_sr_index]) {
			if (GF_M4ASampleRates[cfg->sbr_sr_index] == cfg->sbr_sr)
				break;
			cfg->sbr_sr_index++;
		}
	}
	/*extended object type*/
	if (cfg->base_object_type >= 32) {
		gf_bs_write_int(bs, 31, 5);
		gf_bs_write_int(bs, cfg->base_object_type - 32, 6);
	}
	else {
		gf_bs_write_int(bs, cfg->base_object_type, 5);
	}
	gf_bs_write_int(bs, cfg->base_sr_index, 4);
	if (cfg->base_sr_index == 0x0F) {
		gf_bs_write_int(bs, cfg->base_sr, 24);
	}

	if (cfg->program_config_element_present) {
		gf_bs_write_int(bs, 0, 4);
	} else {
		cfg->chan_cfg = gf_m4a_get_channel_cfg(cfg->nb_chan);
		if (!cfg->chan_cfg) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AAC] Cannot write decoder config, ProgramConfigElement is missing and channel configuration is not a predefined one !\n"));
			return GF_BAD_PARAM;
		}
		gf_bs_write_int(bs, cfg->chan_cfg, 4);
	}

	if (cfg->base_object_type == 5 || cfg->base_object_type == 29) {
		if (cfg->base_object_type == 29) {
			cfg->has_ps = 1;
			cfg->nb_chan = 1;
		}
		cfg->has_sbr = 1;
		gf_bs_write_int(bs, cfg->sbr_sr_index, 4);
		if (cfg->sbr_sr_index == 0x0F) {
			gf_bs_write_int(bs, cfg->sbr_sr, 24);
		}
		gf_bs_write_int(bs, cfg->sbr_object_type, 5);
	}

	/*object cfg*/
	switch (cfg->base_object_type) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 6:
	case 7:
	case 17:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 42:
	{
		/*frame length flag*/
		gf_bs_write_int(bs, 0, 1);
		/*depends on core coder*/
		gf_bs_write_int(bs, 0, 1);
		/*ext flag*/
		gf_bs_write_int(bs, 0, 1);

		if (cfg->program_config_element_present) {
			gf_m4a_write_program_config_element_bs(bs, cfg);
		}

		if ((cfg->base_object_type == 6) || (cfg->base_object_type == 20)) {
			gf_bs_write_int(bs, 0, 3);
		}
	}
	break;
	}
	/*ER cfg - not supported*/

	/*implicit sbr/ps signaling not written here, cf reframe_adts*/
	return GF_OK;
}