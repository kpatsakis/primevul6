Bool gf_hevc_slice_is_IDR(HEVCState *hevc)
{
	if (hevc->sei.recovery_point.valid)
	{
		hevc->sei.recovery_point.valid = 0;
		return GF_TRUE;
	}
	switch (hevc->s_info.nal_unit_type) {
	case GF_HEVC_NALU_SLICE_IDR_W_DLP:
	case GF_HEVC_NALU_SLICE_IDR_N_LP:
		return GF_TRUE;
	default:
		return GF_FALSE;
	}
}