GF_Err vvcc_box_size(GF_Box *s)
{
	u32 i, count, j, subcount;
	GF_VVCConfigurationBox *ptr = (GF_VVCConfigurationBox *)s;

	if (!ptr->config) {
		ptr->size = 0;
		return GF_OK;
	}

	ptr->size += 1;
	if (ptr->config->ptl_present) {
		ptr->size += 3;
		//size PTL
		if (!ptr->config->general_constraint_info)
			ptr->config->num_constraint_info = 0;
		if (!ptr->config->sub_profiles_idc)
			ptr->config->num_sub_profiles = 0;

		ptr->size += 3 + ptr->config->num_constraint_info;
		if (ptr->config->numTemporalLayers>1)
			ptr->size += 1;
		for (i=0; i<ptr->config->numTemporalLayers; i++) {
			if (ptr->config->ptl_sublayer_present_mask & (1<<i))
				ptr->size+=1;
		}
		ptr->size += 1 + ptr->config->num_sub_profiles*4;
		//end size PTL
		ptr->size += 6;
	}

	ptr->size += 1;
	count = gf_list_count(ptr->config->param_array);
	for (i=0; i<count; i++) {
		GF_NALUFFParamArray *ar = (GF_NALUFFParamArray*)gf_list_get(ptr->config->param_array, i);
		if ((ar->type != GF_VVC_NALU_DEC_PARAM) && (ar->type != GF_VVC_NALU_OPI))
			ptr->size += 3;
		else
			ptr->size += 1;

		subcount = gf_list_count(ar->nalus);
		for (j=0; j<subcount; j++) {
			ptr->size += 2 + ((GF_NALUFFParam *)gf_list_get(ar->nalus, j))->size;
		}
	}
	return GF_OK;
}