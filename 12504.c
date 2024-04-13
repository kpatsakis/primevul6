PJ_DEF(pj_status_t) pjmedia_sdp_parse( pj_pool_t *pool,
				       char *buf, pj_size_t len, 
				       pjmedia_sdp_session **p_sdp)
{
    pj_scanner scanner;
    pjmedia_sdp_session *session;
    pjmedia_sdp_media *media = NULL;
    pjmedia_sdp_attr *attr;
    pjmedia_sdp_conn *conn;
    pjmedia_sdp_bandw *bandw;
    pj_str_t dummy;
    int cur_name = 254;
    volatile parse_context ctx;
    PJ_USE_EXCEPTION;

    ctx.last_error = PJ_SUCCESS;

    init_sdp_parser();

    pj_scan_init(&scanner, buf, len, 0, &on_scanner_error);
    session = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_session);
    PJ_ASSERT_RETURN(session != NULL, PJ_ENOMEM);

    /* Ignore leading newlines */
    while (*scanner.curptr=='\r' || *scanner.curptr=='\n')
	pj_scan_get_char(&scanner);

    PJ_TRY {
	while (!pj_scan_is_eof(&scanner)) {
		cur_name = *scanner.curptr;
		switch (cur_name) {
		case 'a':
		    attr = parse_attr(pool, &scanner, &ctx);
		    if (attr) {
			if (media) {
			    if (media->attr_count < PJMEDIA_MAX_SDP_ATTR)
				pjmedia_sdp_media_add_attr(media, attr);
			    else
				PJ_PERROR(2, (THIS_FILE, PJ_ETOOMANY,
					      "Error adding media attribute, "
					      "attribute is ignored"));
			} else {
			    if (session->attr_count < PJMEDIA_MAX_SDP_ATTR)
				pjmedia_sdp_session_add_attr(session, attr);
			    else
				PJ_PERROR(2, (THIS_FILE, PJ_ETOOMANY,
					      "Error adding session attribute"
					      ", attribute is ignored"));
			}
		    }
		    break;
		case 'o':
		    parse_origin(&scanner, session, &ctx);
		    break;
		case 's':
		    parse_generic_line(&scanner, &session->name, &ctx);
		    break;
		case 'c':
		    conn = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_conn);
		    parse_connection_info(&scanner, conn, &ctx);
		    if (media) {
			media->conn = conn;
		    } else {
			session->conn = conn;
		    }
		    break;
		case 't':
		    parse_time(&scanner, session, &ctx);
		    break;
		case 'm':
		    media = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_media);
		    parse_media(&scanner, media, &ctx);
		    if (session->media_count < PJMEDIA_MAX_SDP_MEDIA)
			session->media[ session->media_count++ ] = media;
		    else
			PJ_PERROR(2,(THIS_FILE, PJ_ETOOMANY,
				     "Error adding media, media is ignored"));
		    break;
		case 'v':
		    parse_version(&scanner, &ctx);
		    break;
		case 13:
		case 10:
		    pj_scan_get_char(&scanner);
		    /* Allow empty newlines at the end of the message */
		    while (!pj_scan_is_eof(&scanner)) {
			if (*scanner.curptr != 13 && *scanner.curptr != 10) {
			    ctx.last_error = PJMEDIA_SDP_EINSDP;
			    on_scanner_error(&scanner);
			}
			pj_scan_get_char(&scanner);
		    }
		    break;
		case 'b':
		    bandw = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_bandw);
		    parse_bandwidth_info(&scanner, bandw, &ctx);
		    if (media) {
			if (media->bandw_count < PJMEDIA_MAX_SDP_BANDW)
			    media->bandw[media->bandw_count++] = bandw;
			else
			    PJ_PERROR(2, (THIS_FILE, PJ_ETOOMANY,
					  "Error adding media bandwidth "
					  "info, info is ignored"));
		    } else {
			if (session->bandw_count < PJMEDIA_MAX_SDP_BANDW)
			    session->bandw[session->bandw_count++] = bandw;
			else
			    PJ_PERROR(2, (THIS_FILE, PJ_ETOOMANY,
					  "Error adding session bandwidth "
					  "info, info is ignored"));
		    }
		    break;
		default:
		    if (cur_name >= 'a' && cur_name <= 'z')
			parse_generic_line(&scanner, &dummy, &ctx);
		    else  {
			ctx.last_error = PJMEDIA_SDP_EINSDP;
			on_scanner_error(&scanner);
		    }
		    break;
		}
	}

	ctx.last_error = PJ_SUCCESS;

    }
    PJ_CATCH_ANY {		
	PJ_PERROR(4, (THIS_FILE, ctx.last_error,
		      "Error parsing SDP in line %d col %d",
		      scanner.line, pj_scan_get_col(&scanner)));

	session = NULL;

	pj_assert(ctx.last_error != PJ_SUCCESS);
    }
    PJ_END;

    pj_scan_fini(&scanner);

    if (session)
	apply_media_direction(session);

    *p_sdp = session;
    return ctx.last_error;
}