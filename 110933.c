GF_Err gf_m4a_write_program_config_element_bs(GF_BitStream *bs, GF_M4ADecSpecInfo *cfg)
{
	u32 i;
	gf_bs_write_int(bs, cfg->element_instance_tag, 4);
	gf_bs_write_int(bs, cfg->object_type, 2);
	gf_bs_write_int(bs, cfg->sampling_frequency_index, 4);
	gf_bs_write_int(bs, cfg->num_front_channel_elements, 4);
	gf_bs_write_int(bs, cfg->num_side_channel_elements, 4);
	gf_bs_write_int(bs, cfg->num_back_channel_elements, 4);
	gf_bs_write_int(bs, cfg->num_lfe_channel_elements, 2);
	gf_bs_write_int(bs, cfg->num_assoc_data_elements, 3);
	gf_bs_write_int(bs, cfg->num_valid_cc_elements, 4);
	gf_bs_write_int(bs, cfg->mono_mixdown_present, 1);
	if (cfg->mono_mixdown_present) {
		gf_bs_write_int(bs, cfg->mono_mixdown_element_number, 4);
	}
	gf_bs_write_int(bs, cfg->stereo_mixdown_present, 1);
	if (cfg->stereo_mixdown_present) {
		gf_bs_write_int(bs, cfg->stereo_mixdown_element_number, 4);
	}
	gf_bs_write_int(bs, cfg->matrix_mixdown_idx_present, 1);
	if (cfg->matrix_mixdown_idx_present) {
		gf_bs_write_int(bs, cfg->matrix_mixdown_idx, 2);
		gf_bs_write_int(bs, cfg->pseudo_surround_enable, 1);
	}
	for (i = 0; i < cfg->num_front_channel_elements; i++) {
		gf_bs_write_int(bs, cfg->front_element_is_cpe[i], 1);
		gf_bs_write_int(bs, cfg->front_element_tag_select[i], 4);
	}
	for (i = 0; i < cfg->num_side_channel_elements; i++) {
		gf_bs_write_int(bs, cfg->side_element_is_cpe[i], 1);
		gf_bs_write_int(bs, cfg->side_element_tag_select[i], 4);
	}
	for (i = 0; i < cfg->num_back_channel_elements; i++) {
		gf_bs_write_int(bs, cfg->back_element_is_cpe[i], 1);
		gf_bs_write_int(bs, cfg->back_element_tag_select[i], 4);
	}
	for (i = 0; i < cfg->num_lfe_channel_elements; i++) {
		gf_bs_write_int(bs, cfg->lfe_element_tag_select[i], 4);
	}
	for (i = 0; i < cfg->num_assoc_data_elements; i++) {
		gf_bs_write_int(bs, cfg->assoc_data_element_tag_select[i], 4);
	}

	for (i = 0; i < cfg->num_valid_cc_elements; i++) {
		gf_bs_write_int(bs, cfg->cc_element_is_ind_sw[i], 1);
		gf_bs_write_int(bs, cfg->valid_cc_element_tag_select[i], 4);
	}
	gf_bs_align(bs);
	gf_bs_write_int(bs, cfg->comment_field_bytes, 8);
	gf_bs_write_data(bs, (char *)cfg->comments, cfg->comment_field_bytes);
	return GF_OK;
}