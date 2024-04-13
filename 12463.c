static void init_sdp_parser(void)
{
    if (is_initialized != 0)
	return;

    pj_enter_critical_section();

    if (is_initialized != 0) {
	pj_leave_critical_section();
	return;
    }
    
    pj_cis_buf_init(&cis_buf);

    pj_cis_init(&cis_buf, &cs_token);
    pj_cis_add_alpha(&cs_token);
    pj_cis_add_num(&cs_token);
    pj_cis_add_str(&cs_token, TOKEN);

    pj_cis_init(&cis_buf, &cs_digit);
    pj_cis_add_num(&cs_digit);

    is_initialized = 1;
    pj_leave_critical_section();
}