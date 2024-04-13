int received_request(rad_listen_t *listener,
		     RADIUS_PACKET *packet, REQUEST **prequest,
		     RADCLIENT *client)
{
	RADIUS_PACKET **packet_p;
	REQUEST *request = NULL;
	struct main_config_t *root = &mainconfig;

	packet_p = fr_packet_list_find(pl, packet);
	if (packet_p) {
		request = fr_packet2myptr(REQUEST, packet, packet_p);
		rad_assert(request->in_request_hash);

		if ((request->packet->data_len == packet->data_len) &&
		    (memcmp(request->packet->vector, packet->vector,
			    sizeof(packet->vector)) == 0)) {
			received_retransmit(request, client);
			return 0;
		}

		/*
		 *	The new request is different from the old one,
		 *	but maybe the old is finished.  If so, delete
		 *	the old one.
		 */
		switch (request->child_state) {
			struct timeval when;

		default:
			/*
			 *	Special hacks for race conditions.
			 *	The reply is encoded, and therefore
			 *	likely sent.  We received a *new*
			 *	packet from the client, likely before
			 *	the next line or two of code which
			 *	updated the child state.  In this
			 *	case, just accept the new request.
			 */
			if ((request->reply->code != 0) &&
			    request->reply->data) {
				radlog(L_INFO, "WARNING: Allowing fast client %s port %d - ID: %d for recent request %u.",
				       client->shortname,
				       packet->src_port, packet->id,
				       request->number);
				remove_from_request_hash(request);
				request = NULL;
				break;
			}

			gettimeofday(&when, NULL);
			when.tv_sec -= 1;

			/*
			 *	If the cached request was received
			 *	within the last second, then we
			 *	discard the NEW request instead of the
			 *	old one.  This will happen ONLY when
			 *	the client is severely broken, and is
			 *	sending conflicting packets very
			 *	quickly.
			 */
			if (timercmp(&when, &request->received, <)) {
				radlog(L_ERR, "Discarding conflicting packet from "
				       "client %s port %d - ID: %d due to recent request %u.",
				       client->shortname,
				       packet->src_port, packet->id,
				       request->number);
				return 0;
			}

			received_conflicting_request(request, client);
			request = NULL;
			break;

		case REQUEST_REJECT_DELAY:
		case REQUEST_CLEANUP_DELAY:
			request->child_state = REQUEST_DONE;
		case REQUEST_DONE:
			cleanup_delay(request);
			request = NULL;
			break;
		}
	}

	/*
	 *	We may want to quench the new request.
	 */
	if ((listener->type != RAD_LISTEN_DETAIL) &&
	    !can_handle_new_request(packet, client, root)) {
		return 0;
	}

	/*
	 *	Create and initialize the new request.
	 */
	request = request_alloc(); /* never fails */

	if ((request->reply = rad_alloc(0)) == NULL) {
		radlog(L_ERR, "No memory");
		exit(1);
	}

	request->listener = listener;
	request->client = client;
	request->packet = packet;
	request->packet->timestamp = request->timestamp;
	request->number = request_num_counter++;
	request->priority = listener->type;
#ifdef HAVE_PTHREAD_H
	request->child_pid = NO_SUCH_CHILD_PID;
#endif

	/*
	 *	Status-Server packets go to the head of the queue.
	 */
	if (request->packet->code == PW_STATUS_SERVER) request->priority = 0;

	/*
	 *	Set virtual server identity
	 */
	if (client->server) {
		request->server = client->server;
	} else if (listener->server) {
		request->server = listener->server;
	} else {
		request->server = NULL;
	}

	/*
	 *	Remember the request in the list.
	 */
	if (!fr_packet_list_insert(pl, &request->packet)) {
		radlog(L_ERR, "Failed to insert request %u in the list of live requests: discarding", request->number);
		ev_request_free(&request);
		return 0;
	}

	request->in_request_hash = TRUE;
	request->root = root;
	root->refcount++;

	/*
	 *	The request passes many of our sanity checks.
	 *	From here on in, if anything goes wrong, we
	 *	send a reject message, instead of dropping the
	 *	packet.
	 */

	/*
	 *	Build the reply template from the request.
	 */

	request->reply->sockfd = request->packet->sockfd;
	request->reply->dst_ipaddr = request->packet->src_ipaddr;
	request->reply->src_ipaddr = request->packet->dst_ipaddr;
	request->reply->dst_port = request->packet->src_port;
	request->reply->src_port = request->packet->dst_port;
	request->reply->id = request->packet->id;
	request->reply->code = 0; /* UNKNOWN code */
	memcpy(request->reply->vector, request->packet->vector,
	       sizeof(request->reply->vector));
	request->reply->vps = NULL;
	request->reply->data = NULL;
	request->reply->data_len = 0;

	request->master_state = REQUEST_ACTIVE;
	request->child_state = REQUEST_QUEUED;
	request->next_callback = NULL;

	gettimeofday(&request->received, NULL);
	request->timestamp = request->received.tv_sec;
	request->when = request->received;

	request->delay = USEC;

	tv_add(&request->when, request->delay);

	INSERT_EVENT(wait_a_bit, request);

	*prequest = request;
	return 1;
}