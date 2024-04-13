s32 gf_avc_read_pps_bs(GF_BitStream *bs, AVCState *avc)
{
	return gf_avc_read_pps_bs_internal(bs, avc, 0);
}