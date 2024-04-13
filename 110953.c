static void avc_parse_itu_t_t35_sei(GF_BitStream* bs, AVCSeiItuTT35DolbyVision *dovi)
{
	u8 itu_t_t35_country_code = gf_bs_read_u8(bs);
	u16 terminal_provider_code = gf_bs_read_u16(bs);
	u32 user_id = gf_bs_read_u32(bs);
	u8 data_type_code = gf_bs_read_u8(bs);
	if (itu_t_t35_country_code == 0xB5 && terminal_provider_code == 0x31 && user_id == 0x47413934 && (data_type_code == 0x8 || data_type_code == 0x9)) {
		dovi->rpu_flag = GF_TRUE;
	}
}