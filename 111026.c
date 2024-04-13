static u32 scalability_type_to_idx(HEVC_VPS *vps, u32 scalability_type)
{
	u32 idx = 0, type;
	for (type = 0; type < scalability_type; type++) {
		idx += (vps->scalability_mask[type] ? 1 : 0);
	}
	return idx;
}