GF_Err gf_m4a_parse_config(GF_BitStream *bs, GF_M4ADecSpecInfo *cfg, Bool size_known)
{
	u32 audio_obj_type;
	memset(cfg, 0, sizeof(GF_M4ADecSpecInfo));
	cfg->base_object_type = gf_bs_read_int_log(bs, 5, "base_object_type");
	/*extended object type*/
	if (cfg->base_object_type == 31) {
		cfg->base_object_type = 32 + gf_bs_read_int_log(bs, 6, "extended_base_object_type");
	}
	cfg->base_sr_index = gf_bs_read_int_log(bs, 4, "base_samplerate_index");
	if (cfg->base_sr_index == 0x0F) {
		cfg->base_sr = gf_bs_read_int_log(bs, 24, "base_samplerate");
	}
	else {
		cfg->base_sr = GF_M4ASampleRates[cfg->base_sr_index];
	}

	cfg->chan_cfg = gf_bs_read_int_log(bs, 4, "channel_configuration");
	if (cfg->chan_cfg) {
		cfg->nb_chan = GF_M4ANumChannels[cfg->chan_cfg - 1];
	}

	audio_obj_type = cfg->base_object_type;
	if (cfg->base_object_type == 5 || cfg->base_object_type == 29) {
		if (cfg->base_object_type == 29) {
			cfg->has_ps = 1;
			cfg->nb_chan = 1;
		}
		cfg->has_sbr = GF_TRUE;
		cfg->sbr_sr_index = gf_bs_read_int_log(bs, 4, "sbr_samplerate_index");
		if (cfg->sbr_sr_index == 0x0F) {
			cfg->sbr_sr = gf_bs_read_int_log(bs, 24, "sbr_samplerate");
		}
		else {
			cfg->sbr_sr = GF_M4ASampleRates[cfg->sbr_sr_index];
		}
		cfg->sbr_object_type = gf_bs_read_int_log(bs, 5, "sbr_object_type");
		if (cfg->sbr_object_type==31)
			cfg->sbr_object_type = 32 + gf_bs_read_int_log(bs, 6, "audioObjectTypeExt");
		audio_obj_type = cfg->sbr_object_type;
		if (cfg->sbr_object_type==22) {
			/*ext_chan_cfg = */gf_bs_read_int_log(bs, 4, "channel_configuration");
		}
	}

	/*object cfg*/
	switch (audio_obj_type) {
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
		Bool ext_flag;
		gf_bs_read_int_log(bs, 1, "frame_length_flag");
		if (gf_bs_read_int_log(bs, 1, "depends_on_core_coder"))
			gf_bs_read_int_log(bs, 14, "delay");
		ext_flag = gf_bs_read_int_log(bs, 1, "extension_flag");

		if (!cfg->chan_cfg) {
			gf_m4a_parse_program_config_element(bs, cfg);
		}

		if ((cfg->base_object_type == 6) || (cfg->base_object_type == 20)) {
			gf_bs_read_int_log(bs, 3, "layerN");
		}
		if (ext_flag) {
			if (cfg->base_object_type == 22) {
				gf_bs_read_int_log(bs, 5, "numOfSubFrame");
				gf_bs_read_int_log(bs, 11, "layer_length");
			}
			if ((cfg->base_object_type == 17)
				|| (cfg->base_object_type == 19)
				|| (cfg->base_object_type == 20)
				|| (cfg->base_object_type == 23)
			) {
				gf_bs_read_int_log(bs, 1, "aacSectionDataResilienceFlag");
				gf_bs_read_int_log(bs, 1, "aacScalefactorDataResilienceFlag");
				gf_bs_read_int_log(bs, 1, "aacSpectralDataResilienceFlag");
			}
			gf_bs_read_int_log(bs, 1, "extensionFlag3");
		}
	}
	break;
	}
	/*ER cfg*/
	switch (audio_obj_type) {
	case 17:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	{
		u32 epConfig = gf_bs_read_int_log(bs, 2, "epConfig");
		if ((epConfig == 2) || (epConfig == 3)) {
		}
		if (epConfig == 3) {
			gf_bs_read_int_log(bs, 1, "directMapping");
		}
	}
	break;
	}

	if (size_known && (cfg->base_object_type != 5) && (cfg->base_object_type != 29)) {
		while (gf_bs_available(bs) >= 2) {
			u32 sync = gf_bs_peek_bits(bs, 11, 0);
			if (sync == 0x2b7) {
				gf_bs_read_int_log(bs, 11, "syncExtensionType");
				cfg->sbr_object_type = gf_bs_read_int_log(bs, 5, "extensionAudioObjectType ");
				cfg->has_sbr = gf_bs_read_int_log(bs, 1, "sbrPresentFlag");
				if (cfg->has_sbr) {
					cfg->sbr_sr_index = gf_bs_read_int_log(bs, 4, "extensionSamplingFrequencyIndex");
					if (cfg->sbr_sr_index == 0x0F) {
						cfg->sbr_sr = gf_bs_read_int_log(bs, 24, "extensionSamplingFrequency");
					}
					else {
						cfg->sbr_sr = GF_M4ASampleRates[cfg->sbr_sr_index];
					}
				}
			}
			else if (sync == 0x548) {
				gf_bs_read_int_log(bs, 11, "syncExtensionType");
				cfg->has_ps = gf_bs_read_int_log(bs, 1, "hasParametricStereo");
				if (cfg->has_ps)
					cfg->nb_chan = 1;
			}
			else {
				break;
			}
		}
	}
	cfg->audioPL = gf_m4a_get_profile(cfg);
	return GF_OK;
}