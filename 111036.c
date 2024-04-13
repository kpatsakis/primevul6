static u8 av1_get_qindex(Bool ignoreDeltaQ, u32 segmentId, u32 base_q_idx, u32 delta_q_present, u32 CurrentQIndex, Bool segmentation_enabled, u8 *features_SEG_LVL_ALT_Q_enabled, s32 *features_SEG_LVL_ALT_Q)
{
	//If seg_feature_active_idx( segmentId, SEG_LVL_ALT_Q ) is equal to 1 the following ordered steps apply:
	if (segmentation_enabled && features_SEG_LVL_ALT_Q_enabled[segmentId]) {
		//Set the variable data equal to FeatureData[ segmentId ][ SEG_LVL_ALT_Q ].
		s32 data = features_SEG_LVL_ALT_Q[segmentId];
		s32 qindex = base_q_idx + data;
		//If ignoreDeltaQ is equal to 0 and delta_q_present is equal to 1, set qindex equal to CurrentQIndex + data.
		if ((ignoreDeltaQ == 0) && (delta_q_present == 1)) qindex = CurrentQIndex + data;
		//Return Clip3( 0, 255, qindex ).
		if (qindex < 0) return 0;
		else if (qindex > 255) return 255;
		else return (u8)qindex;
	}
	//Otherwise, if ignoreDeltaQ is equal to 0 and delta_q_present is equal to 1, return CurrentQIndex.
	if ((ignoreDeltaQ == 0) && (delta_q_present == 1)) return CurrentQIndex;
	//otherwise
	return base_q_idx;
}