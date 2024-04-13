static void hevc_scaling_list_data(GF_BitStream *bs)
{
	u32 i, sizeId, matrixId;
	for (sizeId = 0; sizeId < 4; sizeId++) {
		for (matrixId = 0; matrixId < 6; matrixId += (sizeId == 3) ? 3 : 1) {
			u32 idx = sizeId*100 + 10*matrixId;
			u32 scaling_list_pred_mode_flag_sizeId_matrixId = gf_bs_read_int_log_idx(bs, 1, "scaling_list_pred_mode_flag_sizeId_matrixId", idx);
			if (!scaling_list_pred_mode_flag_sizeId_matrixId) {
				gf_bs_read_ue_log_idx(bs, "scaling_list_pred_matrix_id_delta", idx);
			}
			else {
				//u32 nextCoef = 8;
				u32 coefNum = MIN(64, (1 << (4 + (sizeId << 1))));
				if (sizeId > 1) {
					gf_bs_read_se_log_idx(bs, "scaling_list_dc_coef_minus8", idx);
				}
				for (i = 0; i < coefNum; i++) {
					gf_bs_read_se_log_idx2(bs, "scaling_list_delta_coef", idx, i);
				}
			}
		}
	}
}