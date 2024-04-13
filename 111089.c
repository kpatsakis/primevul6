static void avc_hevc_rewrite_vui(GF_VUIInfo *vui_info, GF_BitStream *orig, GF_BitStream *mod)
{
	/* VUI present flag*/
	Bool vui_present_flag = gf_bs_read_int(orig, 1);

	/*setup default values*/
	Bool aspect_ratio_info_present_flag = 0;
	s32 aspect_ratio_idc = -1;
	u32 ar_n=0, ar_d=0;
	Bool overscan_info_present_flag = 0;
	u32 overscan_info=0;
	u32 video_signal_type_present_flag=0;
	u32 video_format = 5;
	u32 video_full_range_flag = 0;
	u32 colour_description_present_flag = 0;
	u32 colour_primaries = 2;
	u32 transfer_characteristics = 2;
	u32 matrix_coefficients = 2;

	//if VUI is present, read all SAR and overscan values
	if (vui_present_flag) { /* VUI found in input bitstream */
		aspect_ratio_info_present_flag = gf_bs_read_int(orig, 1);
		if (aspect_ratio_info_present_flag) {
			aspect_ratio_idc = gf_bs_read_int(orig, 8); /*aspect_ratio_idc*/
			if (aspect_ratio_idc == 255) {
				ar_n = gf_bs_read_int(orig, 16); /*sar_width*/
				ar_d = gf_bs_read_int(orig, 16); /*sar_height*/
			}
		}

		/*overscan_info_present_flag */
		overscan_info_present_flag = gf_bs_read_int(orig, 1);
		if(overscan_info_present_flag) {
			overscan_info = gf_bs_read_int(orig, 1);
		}

		/* read all video signal related flags first */
		video_signal_type_present_flag = gf_bs_read_int(orig, 1);
		if(video_signal_type_present_flag) {
			video_format = gf_bs_read_int(orig, 3);
			video_full_range_flag = gf_bs_read_int(orig, 1);
			colour_description_present_flag = gf_bs_read_int(orig, 1);
			if(colour_description_present_flag) {
				colour_primaries = gf_bs_read_int(orig, 8);
				transfer_characteristics = gf_bs_read_int(orig, 8);
				matrix_coefficients = gf_bs_read_int(orig, 8);
			}
		}
	}

	//recompute values
	//no change
	if ((vui_info->ar_num<0) || (vui_info->ar_den<0)) {
	}
	//remove par
	else if ((vui_info->ar_num==0) || (vui_info->ar_den==0)) {
		aspect_ratio_info_present_flag = 0;
	}
	//set par
	else {
		aspect_ratio_info_present_flag = 1;
		ar_n = vui_info->ar_num;
		ar_d = vui_info->ar_den;
		aspect_ratio_idc = avc_hevc_get_sar_idx((u32) ar_n, (u32) ar_d);
	}

	if (vui_info->remove_video_info) {
		video_signal_type_present_flag = 0;
	}
	/* correct the values of each flags */
	else if ((vui_info->fullrange==0) && (vui_info->video_format==5) && (vui_info->color_prim==2) && (vui_info->color_tfc==2) && (vui_info->color_matrix==2)) {
		video_signal_type_present_flag = 0; /* all default, nothing to write*/
	} else {
		video_signal_type_present_flag = 1;
		video_format = (vui_info->video_format < 0) ? video_format : vui_info->video_format;
		video_full_range_flag = (vui_info->fullrange < 0) ? video_full_range_flag : vui_info->fullrange;
		if ((vui_info->color_prim==2) && (vui_info->color_tfc==2) && (vui_info->color_matrix==2)) {
			colour_description_present_flag = 0;
		} else {
			colour_description_present_flag = 1;
			colour_primaries = (vui_info->color_prim < 0) ? colour_primaries : vui_info->color_prim;
			transfer_characteristics = (vui_info->color_tfc < 0) ? transfer_characteristics : vui_info->color_tfc;
			matrix_coefficients = (vui_info->color_matrix < 0) ? matrix_coefficients : vui_info->color_matrix;
		}
		if ((colour_primaries==2) && (transfer_characteristics==2) && (matrix_coefficients==2)) {
			colour_description_present_flag = 0;
			if ((video_format==5) && (video_full_range_flag==0))
				video_signal_type_present_flag = 0;
		}
	}

	//always rewrite VUI
	gf_bs_write_int(mod, 1, 1);
	gf_bs_write_int(mod, aspect_ratio_info_present_flag, 1);
	if (aspect_ratio_info_present_flag) {
		gf_bs_write_int(mod, aspect_ratio_idc, 8);
		if (aspect_ratio_idc == 255) {
			gf_bs_write_int(mod, ar_n, 16);
			gf_bs_write_int(mod, ar_d, 16);
		}
		if (vui_info->update) {
			vui_info->ar_num = ar_n;
			vui_info->ar_den = ar_d;
		}
	}
	gf_bs_write_int(mod, overscan_info_present_flag, 1);
	if (overscan_info_present_flag) {
		gf_bs_write_int(mod, overscan_info, 1);
	}

	gf_bs_write_int(mod, video_signal_type_present_flag, 1);
	if (video_signal_type_present_flag) {
		gf_bs_write_int(mod, video_format, 3);
		gf_bs_write_int(mod, video_full_range_flag, 1);
		gf_bs_write_int(mod, colour_description_present_flag, 1);

		if (colour_description_present_flag) {
			gf_bs_write_int(mod, colour_primaries, 8);
			gf_bs_write_int(mod, transfer_characteristics, 8);
			gf_bs_write_int(mod, matrix_coefficients, 8);
		}

		if (vui_info->update) {
			vui_info->video_format = video_format;
			vui_info->fullrange = video_full_range_flag;
			if (colour_description_present_flag) {
				vui_info->color_prim = colour_primaries;
				vui_info->color_tfc = transfer_characteristics;
				vui_info->color_matrix = matrix_coefficients;
			}
		}
	}

	/*no VUI in input bitstream but we just inserted one, set all remaining vui flags to 0*/
	if (!vui_present_flag) {
		gf_bs_write_int(mod, 0, 1);		/*chroma_location_info_present_flag */
		gf_bs_write_int(mod, 0, 1);		/*timing_info_present_flag*/
		gf_bs_write_int(mod, 0, 1);		/*nal_hrd_parameters_present*/
		gf_bs_write_int(mod, 0, 1);		/*vcl_hrd_parameters_present*/
		gf_bs_write_int(mod, 0, 1);		/*pic_struct_present*/
		gf_bs_write_int(mod, 0, 1);		/*bitstream_restriction*/
	}
	/*otherwise we copy over th bits from the input bitrate*/
}