static Bool hevc_parse_vps_extension(HEVC_VPS *vps, GF_BitStream *bs)
{
	u8 splitting_flag, vps_nuh_layer_id_present_flag, view_id_len;
	u32 i, j, num_scalability_types, num_add_olss, num_add_layer_set, num_indepentdent_layers, nb_bits, default_output_layer_idc = 0;
	u8 dimension_id_len[16], dim_bit_offset[16];
	u8 /*avc_base_layer_flag, */NumLayerSets, /*default_one_target_output_layer_flag, */rep_format_idx_present_flag, ols_ids_to_ls_idx;
	u8 layer_set_idx_for_ols_minus1[MAX_LHVC_LAYERS];
	u8 nb_output_layers_in_output_layer_set[MAX_LHVC_LAYERS + 1];
	u8 ols_highest_output_layer_id[MAX_LHVC_LAYERS + 1];

	u32 k, d, r, p, iNuhLId, jNuhLId;
	u8 num_direct_ref_layers[64], num_pred_layers[64], num_layers_in_tree_partition[MAX_LHVC_LAYERS];
	u8 dependency_flag[MAX_LHVC_LAYERS][MAX_LHVC_LAYERS], id_pred_layers[64][MAX_LHVC_LAYERS];
	//	u8 num_ref_layers[64];
	//	u8 tree_partition_layer_id[MAX_LHVC_LAYERS][MAX_LHVC_LAYERS];
	//	u8 id_ref_layers[64][MAX_LHVC_LAYERS];
	//	u8 id_direct_ref_layers[64][MAX_LHVC_LAYERS];
	u8 layer_id_in_list_flag[64];
	Bool OutputLayerFlag[MAX_LHVC_LAYERS][MAX_LHVC_LAYERS];

	vps->vps_extension_found = 1;
	if ((vps->max_layers > 1) && vps->base_layer_internal_flag)
		hevc_profile_tier_level(bs, 0, vps->max_sub_layers - 1, &vps->ext_ptl[0], 0);

	splitting_flag = gf_bs_read_int_log(bs, 1, "splitting_flag");
	num_scalability_types = 0;
	for (i = 0; i < 16; i++) {
		vps->scalability_mask[i] = gf_bs_read_int_log_idx(bs, 1, "scalability_mask", i);
		num_scalability_types += vps->scalability_mask[i];
	}
	if (num_scalability_types >= 16) {
		num_scalability_types = 16;
	}
	dimension_id_len[0] = 0;
	for (i = 0; i < (num_scalability_types - splitting_flag); i++) {
		dimension_id_len[i] = 1 + gf_bs_read_int_log_idx(bs, 3, "dimension_id_len_minus1", i);
	}

	if (splitting_flag) {
		for (i = 0; i < num_scalability_types; i++) {
			dim_bit_offset[i] = 0;
			for (j = 0; j < i; j++)
				dim_bit_offset[i] += dimension_id_len[j];
		}
		dimension_id_len[num_scalability_types - 1] = 1 + (5 - dim_bit_offset[num_scalability_types - 1]);
		dim_bit_offset[num_scalability_types] = 6;
	}

	vps_nuh_layer_id_present_flag = gf_bs_read_int_log(bs, 1, "vps_nuh_layer_id_present_flag");
	vps->layer_id_in_nuh[0] = 0;
	vps->layer_id_in_vps[0] = 0;
	for (i = 1; i < vps->max_layers; i++) {
		if (vps_nuh_layer_id_present_flag) {
			vps->layer_id_in_nuh[i] = gf_bs_read_int_log_idx(bs, 6, "layer_id_in_nuh", i);
		}
		else {
			vps->layer_id_in_nuh[i] = i;
		}
		vps->layer_id_in_vps[vps->layer_id_in_nuh[i]] = i;

		if (!splitting_flag) {
			for (j = 0; j < num_scalability_types; j++) {
				vps->dimension_id[i][j] = gf_bs_read_int_log_idx2(bs, dimension_id_len[j], "dimension_id", i, j);
			}
		}
	}

	if (splitting_flag) {
		for (i = 0; i < vps->max_layers; i++)
			for (j = 0; j < num_scalability_types; j++)
				vps->dimension_id[i][j] = ((vps->layer_id_in_nuh[i] & ((1 << dim_bit_offset[j + 1]) - 1)) >> dim_bit_offset[j]);
	}
	else {
		for (j = 0; j < num_scalability_types; j++)
			vps->dimension_id[0][j] = 0;
	}

	view_id_len = gf_bs_read_int_log(bs, 4, "view_id_len");
	if (view_id_len > 0) {
		for (i = 0; i < lhvc_get_num_views(vps); i++) {
			gf_bs_read_int_log_idx(bs, view_id_len, "view_id_val", i);
		}
	}

	for (i = 1; i < vps->max_layers; i++) {
		for (j = 0; j < i; j++) {
			vps->direct_dependency_flag[i][j] = gf_bs_read_int_log_idx(bs, 1, "direct_dependency_flag", i);
		}
	}

	//we do the test on MAX_LHVC_LAYERS and break in the loop to avoid a wrong GCC 4.8 warning on array bounds
	for (i = 0; i < MAX_LHVC_LAYERS; i++) {
		if (i >= vps->max_layers) break;
		for (j = 0; j < vps->max_layers; j++) {
			dependency_flag[i][j] = vps->direct_dependency_flag[i][j];
			for (k = 0; k < i; k++)
				if (vps->direct_dependency_flag[i][k] && vps->direct_dependency_flag[k][j])
					dependency_flag[i][j] = 1;
		}
	}

	for (i = 0; i < vps->max_layers; i++) {
		iNuhLId = vps->layer_id_in_nuh[i];
		d = r = p = 0;
		for (j = 0; j < vps->max_layers; j++) {
			jNuhLId = vps->layer_id_in_nuh[j];
			if (vps->direct_dependency_flag[i][j]) {
				//				id_direct_ref_layers[iNuhLId][d] = jNuhLId;
				d++;
			}
			if (dependency_flag[i][j]) {
				//				id_ref_layers[iNuhLId][r] = jNuhLId;
				r++;
			}

			if (dependency_flag[j][i])
				id_pred_layers[iNuhLId][p++] = jNuhLId;
		}
		num_direct_ref_layers[iNuhLId] = d;
		//		num_ref_layers[iNuhLId] = r;
		num_pred_layers[iNuhLId] = p;
	}

	memset(layer_id_in_list_flag, 0, 64 * sizeof(u8));
	k = 0; //num_indepentdent_layers
	for (i = 0; i < vps->max_layers; i++) {
		iNuhLId = vps->layer_id_in_nuh[i];
		if (!num_direct_ref_layers[iNuhLId]) {
			u32 h = 1;
			//tree_partition_layer_id[k][0] = iNuhLId;
			for (j = 0; j < num_pred_layers[iNuhLId]; j++) {
				u32 predLId = id_pred_layers[iNuhLId][j];
				if (!layer_id_in_list_flag[predLId]) {
					//tree_partition_layer_id[k][h++] = predLId;
					layer_id_in_list_flag[predLId] = 1;
				}
			}
			num_layers_in_tree_partition[k++] = h;
		}
	}
	num_indepentdent_layers = k;

	num_add_layer_set = 0;
	if (num_indepentdent_layers > 1)
		num_add_layer_set = gf_bs_read_ue_log(bs, "num_add_layer_set");

	for (i = 0; i < num_add_layer_set; i++)
		for (j = 1; j < num_indepentdent_layers; j++) {
			nb_bits = 1;
			while ((1 << nb_bits) < (num_layers_in_tree_partition[j] + 1))
				nb_bits++;
			gf_bs_read_int_log_idx2(bs, nb_bits, "highest_layer_idx_plus1", i, j);
		}


	if (gf_bs_read_int_log(bs, 1, "vps_sub_layers_max_minus1_present_flag")) {
		for (i = 0; i < vps->max_layers; i++) {
			gf_bs_read_int_log_idx(bs, 3, "sub_layers_vps_max_minus1", i);
		}
	}

	if (gf_bs_read_int_log(bs, 1, "max_tid_ref_present_flag")) {
		for (i = 0; i < (vps->max_layers - 1); i++) {
			for (j = i + 1; j < vps->max_layers; j++) {
				if (vps->direct_dependency_flag[j][i])
					gf_bs_read_int_log_idx2(bs, 3, "max_tid_il_ref_pics_plus1", i, j);
			}
		}
	}
	gf_bs_read_int_log(bs, 1, "default_ref_layers_active_flag");

	vps->num_profile_tier_level = 1 + gf_bs_read_ue_log(bs, "num_profile_tier_level");
	if (vps->num_profile_tier_level > MAX_LHVC_LAYERS) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[HEVC] Wrong number of PTLs in VPS %d\n", vps->num_profile_tier_level));
		vps->num_profile_tier_level = 1;
		return GF_FALSE;
	}

	for (i = vps->base_layer_internal_flag ? 2 : 1; i < vps->num_profile_tier_level; i++) {
		Bool vps_profile_present_flag = gf_bs_read_int_log_idx(bs, 1, "vps_profile_present_flag", i);
		hevc_profile_tier_level(bs, vps_profile_present_flag, vps->max_sub_layers - 1, &vps->ext_ptl[i - 1], i-1);
	}

	NumLayerSets = vps->num_layer_sets + num_add_layer_set;
	num_add_olss = 0;

	if (NumLayerSets > 1) {
		num_add_olss = gf_bs_read_ue_log(bs, "num_add_olss");
		default_output_layer_idc = gf_bs_read_int_log(bs, 2, "default_output_layer_idc");
		default_output_layer_idc = default_output_layer_idc < 2 ? default_output_layer_idc : 2;
	}
	vps->num_output_layer_sets = num_add_olss + NumLayerSets;

	if (vps->num_output_layer_sets > MAX_LHVC_LAYERS) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[HEVC] Wrong number of output layer sets in VPS %d, max %d supported\n", vps->num_output_layer_sets, MAX_LHVC_LAYERS));
		vps->num_output_layer_sets = 1;
		return GF_FALSE;
	}

	layer_set_idx_for_ols_minus1[0] = 1;
	vps->output_layer_flag[0][0] = 1;

	for (i = 0; i < vps->num_output_layer_sets; i++) {
		if ((NumLayerSets > 2) && (i >= NumLayerSets)) {
			nb_bits = 1;
			while ((1 << nb_bits) < (NumLayerSets - 1))
				nb_bits++;
			layer_set_idx_for_ols_minus1[i] = gf_bs_read_int_log_idx(bs, nb_bits, "layer_set_idx_for_ols_minus1", i);
		}
		else
			layer_set_idx_for_ols_minus1[i] = 0;
		ols_ids_to_ls_idx = i < NumLayerSets ? i : layer_set_idx_for_ols_minus1[i] + 1;

		if ((i > (vps->num_layer_sets - 1)) || (default_output_layer_idc == 2)) {
			for (j = 0; j < vps->num_layers_in_id_list[ols_ids_to_ls_idx]; j++)
				vps->output_layer_flag[i][j] = gf_bs_read_int_log_idx2(bs, 1, "output_layer_flag", i, j);
		}

		if ((default_output_layer_idc == 0) || (default_output_layer_idc == 1)) {
			for (j = 0; j < vps->num_layers_in_id_list[ols_ids_to_ls_idx]; j++) {
				if ((default_output_layer_idc == 0) || (vps->LayerSetLayerIdList[i][j] == vps->LayerSetLayerIdListMax[i]))
					OutputLayerFlag[i][j] = GF_TRUE;
				else
					OutputLayerFlag[i][j] = GF_FALSE;
			}
		}

		for (j = 0; j < vps->num_layers_in_id_list[ols_ids_to_ls_idx]; j++) {
			if (OutputLayerFlag[i][j]) {
				u32 curLayerID;
				vps->necessary_layers_flag[i][j] = GF_TRUE;
				curLayerID = vps->LayerSetLayerIdList[i][j];
				for (k = 0; k < j; k++) {
					u32 refLayerId = vps->LayerSetLayerIdList[i][k];
					if (dependency_flag[vps->layer_id_in_vps[curLayerID]][vps->layer_id_in_vps[refLayerId]])
						vps->necessary_layers_flag[i][k] = GF_TRUE;
				}
			}
		}
		vps->num_necessary_layers[i] = 0;
		for (j = 0; j < vps->num_layers_in_id_list[ols_ids_to_ls_idx]; j++) {
			if (vps->necessary_layers_flag[i][j])
				vps->num_necessary_layers[i] += 1;
		}

		if (i == 0) {
			if (vps->base_layer_internal_flag) {
				if (vps->max_layers > 1)
					vps->profile_tier_level_idx[0][0] = 1;
				else
					vps->profile_tier_level_idx[0][0] = 0;
			}
			continue;
		}
		nb_bits = 1;
		while ((u32)(1 << nb_bits) < vps->num_profile_tier_level)
			nb_bits++;
		for (j = 0; j < vps->num_layers_in_id_list[ols_ids_to_ls_idx]; j++)
			if (vps->necessary_layers_flag[i][j] && vps->num_profile_tier_level)
				vps->profile_tier_level_idx[i][j] = gf_bs_read_int_log_idx2(bs, nb_bits, "profile_tier_level_idx", i, j);
			else
				vps->profile_tier_level_idx[i][j] = 0;


		nb_output_layers_in_output_layer_set[i] = 0;
		for (j = 0; j < vps->num_layers_in_id_list[ols_ids_to_ls_idx]; j++) {
			nb_output_layers_in_output_layer_set[i] += OutputLayerFlag[i][j];
			if (OutputLayerFlag[i][j]) {
				ols_highest_output_layer_id[i] = vps->LayerSetLayerIdList[ols_ids_to_ls_idx][j];
			}
		}
		if (nb_output_layers_in_output_layer_set[i] == 1 && ols_highest_output_layer_id[i] > 0)
			vps->alt_output_layer_flag[i] = gf_bs_read_int_log_idx(bs, 1, "alt_output_layer_flag", i);
	}

	vps->num_rep_formats = 1 + gf_bs_read_ue_log(bs, "num_rep_formats_minus1");
	if (vps->num_rep_formats > 16) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[HEVC] Wrong number of rep formats in VPS %d\n", vps->num_rep_formats));
		vps->num_rep_formats = 0;
		return GF_FALSE;
	}

	for (i = 0; i < vps->num_rep_formats; i++) {
		lhvc_parse_rep_format(&vps->rep_formats[i], bs, i);
	}
	if (vps->num_rep_formats > 1)
		rep_format_idx_present_flag = gf_bs_read_int_log(bs, 1, "rep_format_idx_present_flag");
	else
		rep_format_idx_present_flag = 0;

	vps->rep_format_idx[0] = 0;
	nb_bits = 1;
	while ((u32)(1 << nb_bits) < vps->num_rep_formats)
		nb_bits++;
	for (i = vps->base_layer_internal_flag ? 1 : 0; i < vps->max_layers; i++) {
		if (rep_format_idx_present_flag) {
			vps->rep_format_idx[i] = gf_bs_read_int_log_idx(bs, nb_bits, "rep_format_idx", i);
		}
		else {
			vps->rep_format_idx[i] = i < vps->num_rep_formats - 1 ? i : vps->num_rep_formats - 1;
		}
	}
	//TODO - we don't use the rest ...

	return GF_TRUE;
}