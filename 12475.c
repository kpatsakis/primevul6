PJ_DEF(pj_status_t) pjmedia_sdp_attr_get_ssrc(const pjmedia_sdp_attr *attr,
					      pjmedia_sdp_ssrc_attr *ssrc)
{
    pj_scanner scanner;
    pj_str_t token;
    pj_status_t status = -1;
    PJ_USE_EXCEPTION;

    PJ_ASSERT_RETURN(pj_strcmp2(&attr->name, "ssrc")==0, PJ_EINVALIDOP);

    if (attr->value.slen == 0)
        return PJMEDIA_SDP_EINATTR;

    init_sdp_parser();

    /* ssrc BNF:
     *  a=ssrc:<ssrc-id> <attribute>
     *	a=ssrc:<ssrc-id> <attribute>:<value>
     */

    /* The buffer passed to the scanner is not guaranteed to be NULL
     * terminated, but should be safe. See ticket #2063.
     */
    pj_scan_init(&scanner, (char*)attr->value.ptr, attr->value.slen,
		 PJ_SCAN_AUTOSKIP_WS, &on_scanner_error);

    /* Init */
    pj_bzero(ssrc, sizeof(*ssrc));

    /* Parse */
    PJ_TRY {
        pj_str_t scan_attr;

	/* Get the ssrc */
	pj_scan_get(&scanner, &cs_digit, &token);
	ssrc->ssrc = pj_strtoul(&token);

    	pj_scan_get_char(&scanner);
	pj_scan_get(&scanner, &cs_token, &scan_attr);
	
	/* Get cname attribute, if any */
	if (!pj_scan_is_eof(&scanner) &&
	    pj_scan_get_char(&scanner) == ':' &&
	    pj_strcmp2(&scan_attr, "cname"))
	{
	    pj_scan_get(&scanner, &cs_token, &ssrc->cname);
	}

	status = PJ_SUCCESS;

    }
    PJ_CATCH_ANY {
	status = PJMEDIA_SDP_EINSSRC;
    }
    PJ_END;

    pj_scan_fini(&scanner);
    return status;
}