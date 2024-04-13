GF_Err gf_m4a_parse_program_config_element(GF_BitStream *bs, GF_M4ADecSpecInfo *cfg)
{
	u32 i;

	cfg->program_config_element_present = 1;
	cfg->cpe_channels = 0;

	cfg->element_instance_tag = gf_bs_read_int_log(bs, 4, "element_instance_tag");
	cfg->object_type = gf_bs_read_int_log(bs, 2, "object_type");
	cfg->sampling_frequency_index = gf_bs_read_int_log(bs, 4, "sampling_frequency_index");
	cfg->num_front_channel_elements = gf_bs_read_int_log(bs, 4, "num_front_channel_elements");
	cfg->num_side_channel_elements = gf_bs_read_int_log(bs, 4, "num_side_channel_elements");
	cfg->num_back_channel_elements = gf_bs_read_int_log(bs, 4, "num_back_channel_elements");
	cfg->num_lfe_channel_elements = gf_bs_read_int_log(bs, 2, "num_lfe_channel_elements");
	cfg->num_assoc_data_elements = gf_bs_read_int_log(bs, 3, "num_assoc_data_elements");
	cfg->num_valid_cc_elements = gf_bs_read_int_log(bs, 4, "num_valid_cc_elements");
	cfg->mono_mixdown_present = (Bool)gf_bs_read_int_log(bs, 1, "mono_mixdown_present");
	if (cfg->mono_mixdown_present) {
		cfg->mono_mixdown_element_number = gf_bs_read_int_log(bs, 4, "mono_mixdown_element_number");
	}
	cfg->stereo_mixdown_present = gf_bs_read_int_log(bs, 1, "stereo_mixdown_present");
	if (cfg->stereo_mixdown_present) {
		cfg->stereo_mixdown_element_number = gf_bs_read_int_log(bs, 4, "stereo_mixdown_element_number");
	}
	cfg->matrix_mixdown_idx_present = gf_bs_read_int_log(bs, 1, "matrix_mixdown_idx_present");
	if (cfg->matrix_mixdown_idx_present) {
		cfg->matrix_mixdown_idx = gf_bs_read_int_log(bs, 2, "matrix_mixdown_idx");
		cfg->pseudo_surround_enable = gf_bs_read_int_log(bs, 1, "pseudo_surround_enable");
	}
	for (i = 0; i < cfg->num_front_channel_elements; i++) {
		cfg->front_element_is_cpe[i] = gf_bs_read_int_log_idx(bs, 1, "front_element_is_cpe", i);
		cfg->front_element_tag_select[i] = gf_bs_read_int_log_idx(bs, 4, "front_element_tag_select", i);
		if (cfg->front_element_is_cpe[i]) cfg->cpe_channels++;
	}
	for (i = 0; i < cfg->num_side_channel_elements; i++) {
		cfg->side_element_is_cpe[i] = gf_bs_read_int_log_idx(bs, 1, "side_element_is_cpe", i);
		cfg->side_element_tag_select[i] = gf_bs_read_int_log_idx(bs, 4, "side_element_tag_select", i);
		if (cfg->side_element_is_cpe[i]) cfg->cpe_channels++;
	}
	for (i = 0; i < cfg->num_back_channel_elements; i++) {
		cfg->back_element_is_cpe[i] = gf_bs_read_int_log_idx(bs, 1, "back_element_is_cpe", i);
		cfg->back_element_tag_select[i] = gf_bs_read_int_log_idx(bs, 4, "back_element_tag_select", i);
		if (cfg->back_element_is_cpe[i]) cfg->cpe_channels++;
	}
	for (i = 0; i < cfg->num_lfe_channel_elements; i++) {
		cfg->lfe_element_tag_select[i] = gf_bs_read_int_log_idx(bs, 4, "lfe_element_tag_select", i);
	}
	for (i = 0; i < cfg->num_assoc_data_elements; i++) {
		cfg->assoc_data_element_tag_select[i] = gf_bs_read_int_log_idx(bs, 4, "assoc_data_element_tag_select", i);
	}

	for (i = 0; i < cfg->num_valid_cc_elements; i++) {
		cfg->cc_element_is_ind_sw[i] = gf_bs_read_int_log_idx(bs, 1, "cc_element_is_ind_sw", i);
		cfg->valid_cc_element_tag_select[i] = gf_bs_read_int_log_idx(bs, 4, "valid_cc_element_tag_select", i);
	}
	gf_bs_align(bs);
	cfg->comment_field_bytes = gf_bs_read_int_log(bs, 8, "comment_field_bytes");
	gf_bs_read_data(bs, (char *)cfg->comments, cfg->comment_field_bytes);

	cfg->nb_chan = cfg->num_front_channel_elements + cfg->num_back_channel_elements + cfg->num_side_channel_elements + cfg->num_lfe_channel_elements;
	cfg->nb_chan += cfg->cpe_channels;

	return GF_OK;
}