static void hevc_compute_poc(HEVCSliceInfo *si)
{
	u32 max_poc_lsb = 1 << (si->sps->log2_max_pic_order_cnt_lsb);

	/*POC reset for IDR frames, NOT for CRA*/
	switch (si->nal_unit_type) {
	case GF_HEVC_NALU_SLICE_IDR_W_DLP:
	case GF_HEVC_NALU_SLICE_IDR_N_LP:
		si->poc_lsb_prev = 0;
		si->poc_msb_prev = 0;
		break;
	}

	if ((si->poc_lsb < si->poc_lsb_prev) && (si->poc_lsb_prev - si->poc_lsb >= max_poc_lsb / 2))
		si->poc_msb = si->poc_msb_prev + max_poc_lsb;
	else if ((si->poc_lsb > si->poc_lsb_prev) && (si->poc_lsb - si->poc_lsb_prev > max_poc_lsb / 2))
		si->poc_msb = si->poc_msb_prev - max_poc_lsb;
	else
		si->poc_msb = si->poc_msb_prev;

	switch (si->nal_unit_type) {
	case GF_HEVC_NALU_SLICE_BLA_W_LP:
	case GF_HEVC_NALU_SLICE_BLA_W_DLP:
	case GF_HEVC_NALU_SLICE_BLA_N_LP:
		si->poc_msb = 0;
		break;
	}
	si->poc = si->poc_msb + si->poc_lsb;
}