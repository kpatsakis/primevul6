static u32 lhvc_get_num_views(HEVC_VPS *vps)
{
	u32 numViews = 1, i;
	for (i = 0; i < vps->max_layers; i++) {
		u32 layer_id = vps->layer_id_in_nuh[i];
		if (i > 0 && (lhvc_get_view_index(vps, layer_id) != lhvc_get_scalability_id(vps, i - 1, LHVC_VIEW_ORDER_INDEX))) {
			numViews++;
		}
	}
	return numViews;
}