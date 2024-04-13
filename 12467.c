PJ_DEF(pj_status_t) pjmedia_sdp_validate2(const pjmedia_sdp_session *sdp,
					  pj_bool_t strict)
{
    unsigned i;
    const pj_str_t STR_RTPMAP = { "rtpmap", 6 };

    CHECK( sdp != NULL, PJ_EINVAL);

    /* Validate origin line. */
    CHECK( sdp->origin.user.slen != 0, PJMEDIA_SDP_EINORIGIN);
    CHECK( pj_strcmp2(&sdp->origin.net_type, "IN")==0, 
	   PJMEDIA_SDP_EINORIGIN);
    CHECK( pj_strcmp2(&sdp->origin.addr_type, "IP4")==0 ||
	   pj_strcmp2(&sdp->origin.addr_type, "IP6")==0, 
	   PJMEDIA_SDP_EINORIGIN);
    CHECK( sdp->origin.addr.slen != 0, PJMEDIA_SDP_EINORIGIN);

    /* Validate subject line. */
    CHECK( sdp->name.slen != 0, PJMEDIA_SDP_EINNAME);

    /* Ignore start and stop time. */

    /* If session level connection info is present, validate it. */
    if (sdp->conn) {
	pj_status_t status = validate_sdp_conn(sdp->conn);
	if (status != PJ_SUCCESS)
	    return status;
    }

    /* Validate each media. */
    for (i=0; i<sdp->media_count; ++i) {
	const pjmedia_sdp_media *m = sdp->media[i];
	unsigned j;

	/* Validate the m= line. */
	CHECK( m->desc.media.slen != 0, PJMEDIA_SDP_EINMEDIA);
	CHECK( m->desc.transport.slen != 0, PJMEDIA_SDP_EINMEDIA);
	CHECK( m->desc.fmt_count != 0 || m->desc.port==0, PJMEDIA_SDP_ENOFMT);

	/* If media level connection info is present, validate it. */
	if (m->conn) {
	    pj_status_t status = validate_sdp_conn(m->conn);
	    if (status != PJ_SUCCESS)
		return status;
	}

	/* If media doesn't have connection info, then connection info
	 * must be present in the session.
	 */
	if (m->conn == NULL) {
	    if (sdp->conn == NULL)
		if (strict || m->desc.port != 0)
		    return PJMEDIA_SDP_EMISSINGCONN;
	}

	/* Verify payload type. */
	for (j=0; j<m->desc.fmt_count; ++j) {

	    /* Arrgh noo!! Payload type can be non-numeric!!
	     * RTC based programs sends "null" for instant messaging!
	     */
	    if (pj_isdigit(*m->desc.fmt[j].ptr)) {
		unsigned long pt;
		pj_status_t status = pj_strtoul3(&m->desc.fmt[j], &pt, 10);

		/* Payload type is between 0 and 127. 
		 */
		CHECK( status == PJ_SUCCESS && pt <= 127, PJMEDIA_SDP_EINPT);

		/* If port is not zero, then for each dynamic payload type, an
		 * rtpmap attribute must be specified.
		 */
		if (m->desc.port != 0 && pt >= 96) {
		    const pjmedia_sdp_attr *a;

		    a = pjmedia_sdp_media_find_attr(m, &STR_RTPMAP, 
						    &m->desc.fmt[j]);
		    CHECK( a != NULL, PJMEDIA_SDP_EMISSINGRTPMAP);
		}
	    }
	}
    }

    /* Looks good. */
    return PJ_SUCCESS;
}