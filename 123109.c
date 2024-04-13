static void received_conflicting_request(REQUEST *request,
					 const RADCLIENT *client)
{
	radlog(L_ERR, "Received conflicting packet from "
	       "client %s port %d - ID: %d due to unfinished request %u.  Giving up on old request.",
	       client->shortname,
	       request->packet->src_port, request->packet->id,
	       request->number);

	/*
	 *	Nuke it from the request hash, so we can receive new
	 *	packets.
	 */
	remove_from_request_hash(request);

	switch (request->child_state) {
#ifdef HAVE_PTHREAD_H
		/*
		 *	It's queued or running.  Tell it to stop, and
		 *	wait for it to do so.
		 */
	case REQUEST_QUEUED:
	case REQUEST_RUNNING:
		request->master_state = REQUEST_STOP_PROCESSING;
		request->delay += request->delay >> 1;

		tv_add(&request->when, request->delay);

		INSERT_EVENT(wait_for_child_to_die, request);
		return;
#endif

		/*
		 *	Catch race conditions.  It may have switched
		 *	from running to done while this code is being
		 *	executed.
		 */
	case REQUEST_REJECT_DELAY:
	case REQUEST_CLEANUP_DELAY:
	case REQUEST_DONE:
		break;

		/*
		 *	It's in some other state, and therefore also
		 *	in the event queue.  At some point, the
		 *	child will notice, and we can then delete it.
		 */
	case REQUEST_PROXIED:
	default:
		rad_assert(request->ev != NULL);
		break;
	}
}