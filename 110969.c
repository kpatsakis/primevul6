static void operating_parameters_info(GF_BitStream *bs, const u8 idx, const u8 buffer_delay_length_minus_1) {
	const u8 n = buffer_delay_length_minus_1 + 1;
	gf_bs_read_int_log(bs, n, "decoder_buffer_delay");
	gf_bs_read_int_log(bs, n, "encoder_buffer_delay");
	gf_bs_read_int_log(bs, 1, "low_delay_mode_flag");
}