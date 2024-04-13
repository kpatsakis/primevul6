static s32 SVC_ReadNal_header_extension(GF_BitStream *bs, SVC_NALUHeader *NalHeader)
{
	gf_bs_read_int_log(bs, 1, "reserved_one_bit");
	NalHeader->idr_pic_flag = gf_bs_read_int_log(bs, 1, "idr_flag");
	NalHeader->priority_id = gf_bs_read_int_log(bs, 6, "priority_id");
	gf_bs_read_int_log(bs, 1, "no_inter_layer_pred_flag");
	NalHeader->dependency_id = gf_bs_read_int_log(bs, 3, "DependencyId");
	NalHeader->quality_id = gf_bs_read_int_log(bs, 4, "quality_id");
	NalHeader->temporal_id = gf_bs_read_int_log(bs, 3, "temporal_id");
	gf_bs_read_int_log(bs, 1, "use_ref_base_pic_flag");
	gf_bs_read_int_log(bs, 1, "discardable_flag");
	gf_bs_read_int_log(bs, 1, "output_flag");
	gf_bs_read_int_log(bs, 2, "reserved_three_2bits");
	return 1;
}