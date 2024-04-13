static void event_poll_detail(void *ctx)
{
	int rcode, delay;
	RAD_REQUEST_FUNP fun;
	REQUEST *request;
	rad_listen_t *this = ctx;
	struct timeval when;
	listen_detail_t *detail = this->data;

	rad_assert(this->type == RAD_LISTEN_DETAIL);

	/*
	 *	Try to read something.
	 *
	 *	FIXME: This does poll AND receive.
	 */
	rcode = this->recv(this, &fun, &request);
	if (rcode != 0) {
		rad_assert(fun != NULL);
		rad_assert(request != NULL);
		
		if (!thread_pool_addrequest(request, fun)) {
			request->child_state = REQUEST_DONE;
		}
	}

	if (!fr_event_now(el, &now)) gettimeofday(&now, NULL);
	when = now;

	/*
	 *	Backdoor API to get the delay until the next poll
	 *	time.
	 */
	delay = this->encode(this, NULL);
	tv_add(&when, delay);

	if (!fr_event_insert(el, event_poll_detail, this,
			     &when, &detail->ev)) {
		radlog(L_ERR, "Failed creating handler");
		exit(1);
	}
}