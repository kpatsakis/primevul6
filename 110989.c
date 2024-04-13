Bool gf_hevc_slice_is_intra(HEVCState *hevc)
{
	switch (hevc->s_info.nal_unit_type) {
	case GF_HEVC_NALU_SLICE_BLA_W_LP:
	case GF_HEVC_NALU_SLICE_BLA_W_DLP:
	case GF_HEVC_NALU_SLICE_BLA_N_LP:
	case GF_HEVC_NALU_SLICE_IDR_W_DLP:
	case GF_HEVC_NALU_SLICE_IDR_N_LP:
	case GF_HEVC_NALU_SLICE_CRA:
		return GF_TRUE;
	default:
		return GF_FALSE;
	}
}