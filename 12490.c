static void parse_bandwidth_info(pj_scanner *scanner, pjmedia_sdp_bandw *bandw,
				 volatile parse_context *ctx)
{
    pj_str_t str;

    ctx->last_error = PJMEDIA_SDP_EINBANDW;

    /* b= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* modifier */
    pj_scan_get_until_ch(scanner, ':', &bandw->modifier);
    pj_scan_get_char(scanner);

    /* value */
    pj_scan_get_until_chr(scanner, " \t\r\n", &str);
    bandw->value = pj_strtoul(&str);

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);
}