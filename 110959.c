static u32 lhvc_get_view_index(HEVC_VPS *vps, u32 id)
{
	return lhvc_get_scalability_id(vps, vps->layer_id_in_vps[id], LHVC_VIEW_ORDER_INDEX);
}