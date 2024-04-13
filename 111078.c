static u32 lhvc_get_scalability_id(HEVC_VPS *vps, u32 layer_id_in_vps, u32 scalability_type)
{
	u32 idx;
	if (!vps->scalability_mask[scalability_type]) return 0;
	idx = scalability_type_to_idx(vps, scalability_type);
	return vps->dimension_id[layer_id_in_vps][idx];
}