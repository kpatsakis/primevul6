static void vp9_quantization_params(GF_BitStream *bs)
{
	/*base_q_idx = */gf_bs_read_int_log(bs, 8, "base_q_idx");
}