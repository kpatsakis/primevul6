static int setup_post_proxy_fail(REQUEST *request)
{
	DICT_VALUE *dval = NULL;
	VALUE_PAIR *vp;

	request->child_state = REQUEST_RUNNING;

	if (request->packet->code == PW_AUTHENTICATION_REQUEST) {
		dval = dict_valbyname(PW_POST_PROXY_TYPE, "Fail-Authentication");

	} else if (request->packet->code == PW_ACCOUNTING_REQUEST) {
		dval = dict_valbyname(PW_POST_PROXY_TYPE, "Fail-Accounting");

#ifdef WITH_COA
		/*
		 *	See no_response_to_coa_request
		 */
	} else if (((request->packet->code >> 8) & 0xff) == PW_COA_REQUEST) {
		request->packet->code &= 0xff; /* restore it */

		if (request->proxy->code == PW_COA_REQUEST) {
			dval = dict_valbyname(PW_POST_PROXY_TYPE, "Fail-CoA");

		} else if (request->proxy->code == PW_DISCONNECT_REQUEST) {
			dval = dict_valbyname(PW_POST_PROXY_TYPE, "Fail-Disconnect");
		} else {
			return 0;
		}

#endif
	} else {
		return 0;
	}

	if (!dval) dval = dict_valbyname(PW_POST_PROXY_TYPE, "Fail");

	if (!dval) {
		pairdelete(&request->config_items, PW_POST_PROXY_TYPE);
		return 0;
	}

	vp = pairfind(request->config_items, PW_POST_PROXY_TYPE);
	if (!vp) vp = radius_paircreate(request, &request->config_items,
					PW_POST_PROXY_TYPE, PW_TYPE_INTEGER);
	vp->vp_integer = dval->value;

	rad_assert(request->proxy_reply == NULL);

	return 1;
}