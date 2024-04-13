static s32 avc_parse_recovery_point_sei(GF_BitStream *bs, AVCState *avc)
{
	AVCSeiRecoveryPoint *rp = &avc->sei.recovery_point;

	rp->frame_cnt = gf_bs_read_ue_log(bs, "frame_cnt");
	rp->exact_match_flag = gf_bs_read_int_log(bs, 1, "exact_match_flag");
	rp->broken_link_flag = gf_bs_read_int_log(bs, 1, "broken_link_flag");
	rp->changing_slice_group_idc = gf_bs_read_int_log(bs, 2, "changing_slice_group_idc");
	rp->valid = 1;

	return 0;
}