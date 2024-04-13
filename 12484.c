static void apply_media_direction(pjmedia_sdp_session *sdp)
{
    pjmedia_sdp_attr *dir_attr = NULL;
    unsigned i;

    const pj_str_t inactive = { "inactive", 8 };
    const pj_str_t sendonly = { "sendonly", 8 };
    const pj_str_t recvonly = { "recvonly", 8 };
    const pj_str_t sendrecv = { "sendrecv", 8 };

    /* Find direction attribute in session, don't need to find default 
     * direction "sendrecv".
     */
    for (i = 0; i < sdp->attr_count && !dir_attr; ++i) {
	if (!pj_strcmp(&sdp->attr[i]->name, &sendonly) ||
	    !pj_strcmp(&sdp->attr[i]->name, &recvonly) ||
	    !pj_strcmp(&sdp->attr[i]->name, &inactive)) 
	{
	    dir_attr = sdp->attr[i];
	}
    }

    /* Found the direction attribute */
    if (dir_attr) {
	/* Remove the direction attribute in session */
	pjmedia_sdp_attr_remove(&sdp->attr_count, sdp->attr, dir_attr);

	/* Apply the direction attribute to all media, but not overriding it
	 * if media already has direction attribute.
	 */
	for (i = 0; i < sdp->media_count; ++i) {
	    pjmedia_sdp_media *m;
	    unsigned j;

	    /* Find direction attribute in this media */
	    m = sdp->media[i];
	    for (j = 0; j < m->attr_count; ++j) {
		if (!pj_strcmp(&m->attr[j]->name, &sendrecv) ||
		    !pj_strcmp(&m->attr[j]->name, &sendonly) ||
		    !pj_strcmp(&m->attr[j]->name, &recvonly) ||
		    !pj_strcmp(&m->attr[j]->name, &inactive)) 
		{
		    break;
		}
	    }

	    /* Not found, apply direction attribute from session */
	    if (j == m->attr_count)
		pjmedia_sdp_media_add_attr(m, dir_attr);
	}
    }
}