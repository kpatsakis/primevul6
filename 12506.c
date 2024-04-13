PJ_DEF(pj_status_t) pjmedia_sdp_attr_get_rtpmap( const pjmedia_sdp_attr *attr,
						 pjmedia_sdp_rtpmap *rtpmap)
{
    pj_scanner scanner;
    pj_str_t token;
    pj_status_t status = -1;
    char term = 0;
    PJ_USE_EXCEPTION;

    PJ_ASSERT_RETURN(pj_strcmp2(&attr->name, "rtpmap")==0, PJ_EINVALIDOP);

    if (attr->value.slen == 0)
        return PJMEDIA_SDP_EINATTR;

    init_sdp_parser();

    /* Check if input is null terminated, and null terminate if
     * necessary. Unfortunately this may crash the application if
     * attribute was allocated from a read-only memory location.
     * But this shouldn't happen as attribute's value normally is
     * null terminated.
     */
    if (attr->value.ptr[attr->value.slen] != 0 &&
	attr->value.ptr[attr->value.slen] != '\r' &&
	attr->value.ptr[attr->value.slen] != '\n')
    {
	pj_assert(!"Shouldn't happen");
	term = attr->value.ptr[attr->value.slen];
	attr->value.ptr[attr->value.slen] = '\0';
    }

    /* The buffer passed to the scanner is not guaranteed to be NULL
     * terminated, but should be safe. See ticket #2063.
     */    
    pj_scan_init(&scanner, (char*)attr->value.ptr, attr->value.slen,
		 PJ_SCAN_AUTOSKIP_WS, &on_scanner_error);

    /* rtpmap sample:
     *	a=rtpmap:98 L16/16000/2.
     */

    /* Init */
    rtpmap->pt.slen = rtpmap->param.slen = rtpmap->enc_name.slen = 0;
    rtpmap->clock_rate = 0;

    /* Parse */
    PJ_TRY {

	/* Get payload type. */
	pj_scan_get(&scanner, &cs_token, &rtpmap->pt);


	/* Get encoding name. */
	pj_scan_get(&scanner, &cs_token, &rtpmap->enc_name);

	/* Expecting '/' after encoding name. */
	if (pj_scan_get_char(&scanner) != '/') {
	    status = PJMEDIA_SDP_EINRTPMAP;
	    goto on_return;
	}


	/* Get the clock rate. */
	pj_scan_get(&scanner, &cs_digit, &token);
	rtpmap->clock_rate = pj_strtoul(&token);

	/* Expecting either '/' or EOF */
	if (*scanner.curptr == '/') {
	    /* Skip the '/' */
	    pj_scan_get_char(&scanner);
	    pj_scan_get(&scanner, &cs_token, &rtpmap->param);
	} else {
	    rtpmap->param.slen = 0;
	}

	status = PJ_SUCCESS;
    }
    PJ_CATCH_ANY {
	status = PJMEDIA_SDP_EINRTPMAP;
    }
    PJ_END;


on_return:
    pj_scan_fini(&scanner);
    if (term) {
	attr->value.ptr[attr->value.slen] = term;
    }
    return status;
}