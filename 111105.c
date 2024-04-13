static GF_Err vp9_color_config(GF_BitStream *bs, GF_VPConfig *vp9_cfg)
{
	VP9_color_space color_space;

	if (vp9_cfg->profile >= 2) {
		Bool ten_or_twelve_bit = gf_bs_read_int_log(bs, 1, "ten_or_twelve_bit");
		vp9_cfg->bit_depth = ten_or_twelve_bit ? 12 : 10;
	}
	else {
		vp9_cfg->bit_depth = 8;
	}

	color_space = gf_bs_read_int_log(bs, 3, "color_space");
	vp9_cfg->colour_primaries = VP9_CS_to_23001_8_colour_primaries[color_space];
	vp9_cfg->transfer_characteristics = VP9_CS_to_23001_8_transfer_characteristics[color_space];
	vp9_cfg->matrix_coefficients = VP9_CS_to_23001_8_matrix_coefficients[color_space];
	if (color_space != CS_RGB) {
		vp9_cfg->video_fullRange_flag = gf_bs_read_int_log(bs, 1, "video_fullRange_flag");
		if (vp9_cfg->profile == 1 || vp9_cfg->profile == 3) {
			u8 subsampling_x, subsampling_y, subsampling_xy_to_chroma_subsampling[2][2] = { {3, 0}, {2, 0} };
			subsampling_x = gf_bs_read_int_log(bs, 1, "subsampling_x");
			subsampling_y = gf_bs_read_int_log(bs, 1, "subsampling_x");
			vp9_cfg->chroma_subsampling = subsampling_xy_to_chroma_subsampling[subsampling_x][subsampling_y];
			Bool reserved_zero = gf_bs_read_int_log(bs, 1, "reserved_zero");
			if (reserved_zero) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VP9] color config reserved zero (1) is not zero.\n"));
				return GF_NON_COMPLIANT_BITSTREAM;
			}
		}
		else {
			vp9_cfg->chroma_subsampling = 0;
		}
	}
	else {
		vp9_cfg->video_fullRange_flag = GF_TRUE;
		if (vp9_cfg->profile == 1 || vp9_cfg->profile == 3) {
			vp9_cfg->chroma_subsampling = 3;
			Bool reserved_zero = gf_bs_read_int_log(bs, 1, "reserved_zero");
			if (reserved_zero) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VP9] color config reserved zero (2) is not zero.\n"));
				return GF_NON_COMPLIANT_BITSTREAM;
			}
		}
	}

	return GF_OK;
}