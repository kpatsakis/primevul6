s32 gf_avc_read_sps_bs(GF_BitStream *bs, AVCState *avc, u32 subseq_sps, u32 *vui_flag_pos)
{
	return gf_avc_read_sps_bs_internal(bs, avc, subseq_sps, vui_flag_pos, 0);
}