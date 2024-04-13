static void parse_connection_info(pj_scanner *scanner, pjmedia_sdp_conn *conn,
				  volatile parse_context *ctx)
{
    ctx->last_error = PJMEDIA_SDP_EINCONN;

    /* c= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* network-type */
    pj_scan_get_until_ch(scanner, ' ', &conn->net_type);
    pj_scan_get_char(scanner);

    /* addr-type */
    pj_scan_get_until_ch(scanner, ' ', &conn->addr_type);
    pj_scan_get_char(scanner);

    /* address. */
    pj_scan_get_until_chr(scanner, "/ \t\r\n", &conn->addr);
    PJ_TODO(PARSE_SDP_CONN_ADDRESS_SUBFIELDS);

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);
}