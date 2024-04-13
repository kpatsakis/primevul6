void gf_vvc_parse_ps(GF_VVCConfig* vvccfg, VVCState* vvc, u32 nal_type)
{
	u32 i, j;
	if (!vvccfg) return;

	for (i = 0; i < gf_list_count(vvccfg->param_array); i++) {
		GF_NALUFFParamArray* ar = gf_list_get(vvccfg->param_array, i);
		if (ar->type != nal_type) continue;
		for (j = 0; j < gf_list_count(ar->nalus); j++) {
			u8 ntype, tid, lid;
			GF_NALUFFParam* sl = gf_list_get(ar->nalus, j);
			gf_vvc_parse_nalu(sl->data, sl->size, vvc, &ntype, &tid, &lid);
		}
	}
}