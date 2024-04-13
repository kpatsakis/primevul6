REQUEST *received_proxy_response(RADIUS_PACKET *packet)
{
	char		buffer[128];
	REQUEST		*request;

	/*
	 *	Also removes from the proxy hash if responses == requests
	 */
	request = lookup_in_proxy_hash(packet);

	if (!request) {
		radlog(L_PROXY, "No outstanding request was found for reply from host %s port %d - ID %d",
		       inet_ntop(packet->src_ipaddr.af,
				 &packet->src_ipaddr.ipaddr,
				 buffer, sizeof(buffer)),
		       packet->src_port, packet->id);
		return NULL;
	}

	/*
	 *	We haven't replied to the NAS, but we have seen an
	 *	earlier reply from the home server.  Ignore this packet,
	 *	as we're likely still processing the previous reply.
	 */
	if (request->proxy_reply) {
		if (memcmp(request->proxy_reply->vector,
			   packet->vector,
			   sizeof(request->proxy_reply->vector)) == 0) {
			RDEBUG2("Discarding duplicate reply from host %s port %d  - ID: %d for request %u",
			       inet_ntop(packet->src_ipaddr.af,
					 &packet->src_ipaddr.ipaddr,
					 buffer, sizeof(buffer)),
			       packet->src_port, packet->id,
			       request->number);
		} else {
			/*
			 *	? The home server gave us a new proxy
			 *	reply which doesn't match the old
			 *	one.  Delete it.
			 */
			RDEBUG2("Ignoring conflicting proxy reply");
		}
		
		/* assert that there's an event queued for request? */
		return NULL;
	}

	/*
	 *	Verify the packet before doing ANYTHING with it.  This
	 *	means we're doing more MD5 checks in the server core.
	 *	However, we can fix that by moving to multiple threads
	 *	listening on sockets.
	 *
	 *	We do this AFTER looking the request up in the hash,
	 *	and AFTER vhecking if we saw a previous request.  This
	 *	helps minimize the DoS effect of people attacking us
	 *	with spoofed packets.
	 */
	if (rad_verify(packet, request->proxy,
		       request->home_server->secret) != 0) {
		DEBUG("Ignoring spoofed proxy reply.  Signature is invalid");
		return NULL;
	}

	gettimeofday(&now, NULL);

	/*
	 *	"ping" packets have a different algorithm for marking
	 *	a home server alive.  They also skip all of the CoA,
	 *	etc. checks.
	 */
	if (!request->packet) {
		request->proxy_reply = packet;
		received_response_to_ping(request);
		request->proxy_reply = NULL; /* caller will free it */
		ev_request_free(&request);
		return NULL;
	}

	/*
	 *	Maybe move this earlier in the decision process?
	 *	Having it here means that late or duplicate proxy
	 *	replies no longer get the home server marked as
	 *	"alive".  This might be good for stability, though.
	 *
	 *	FIXME: Do we really want to do this whenever we
	 *	receive a packet?  Setting this here means that we
	 *	mark it alive on *any* packet, even if it's lost all
	 *	of the *other* packets in the last 10s.
	 */
	request->home_server->state = HOME_STATE_ALIVE;
	
#ifdef WITH_COA
	/*
	 *	When originating CoA, the "proxy" reply is the reply
	 *	to the CoA request that we originated.  At this point,
	 *	the original request is finished, and it has a reply.
	 *
	 *	However, if we haven't separated the two requests, do
	 *	so now.  This is done so that cleaning up the original
	 *	request won't cause the CoA request to be free'd.  See
	 *	util.c, request_free()
	 */
	if (request->parent && (request->parent->coa == request)) {
		request->parent->coa = NULL;
		request->parent = NULL;

		/*
		 *	The proxied packet was different from the
		 *	original packet, AND the proxied packet was
		 *	a CoA: allow it.
		 */
	} else if ((request->packet->code != request->proxy->code) &&
		   ((request->proxy->code == PW_COA_REQUEST) ||
		    (request->proxy->code == PW_DISCONNECT_REQUEST))) {
	  /*
	   *	It's already divorced: do nothing.
	   */
	  
	} else
		/*
		 *	Skip the next set of checks, as the original
		 *	reply is cached.  We want to be able to still
		 *	process the CoA reply, AND to reference the
		 *	original request/reply.
		 *
		 *	This is getting to be really quite a bit of a
		 *	hack.
		 */
#endif

	/*
	 *	If there's a reply to the NAS, ignore everything
	 *	related to proxy responses
	 */
	if (request->reply && request->reply->code != 0) {
		RDEBUG2("Ignoring proxy reply that arrived after we sent a reply to the NAS");
		return NULL;
	}

#ifdef WITH_STATS
	/*
	 *	The average includes our time to receive packets and
	 *	look them up in the hashes, which should be the same
	 *	for all packets.
	 *
	 *	We update the response time only for the FIRST packet
	 *	we receive.
	 */
	if (request->home_server->ema.window > 0) {
		radius_stats_ema(&request->home_server->ema,
				 &now, &request->proxy_when);
	}
#endif

	switch (request->child_state) {
	case REQUEST_QUEUED:
	case REQUEST_RUNNING:
		radlog(L_ERR, "Internal sanity check failed for child state");
		/* FALL-THROUGH */

	case REQUEST_REJECT_DELAY:
	case REQUEST_CLEANUP_DELAY:
	case REQUEST_DONE:
		radlog(L_ERR, "Reply from home server %s port %d  - ID: %d arrived too late for request %u. Try increasing 'retry_delay' or 'max_request_time'",
		       inet_ntop(packet->src_ipaddr.af,
				 &packet->src_ipaddr.ipaddr,
				 buffer, sizeof(buffer)),
		       packet->src_port, packet->id,
		       request->number);
		/* assert that there's an event queued for request? */
		return NULL;

	case REQUEST_PROXIED:
		break;
	}

	request->proxy_reply = packet;

#if 0
	/*
	 *	Perform RTT calculations, as per RFC 2988 (for TCP).
	 *	Note that we only do so on the first response.
	 */
	if ((request->num_proxied_responses == 1)
		int rtt;
		home_server *home = request->home_server;

		rtt = now.tv_sec - request->proxy_when.tv_sec;
		rtt *= USEC;
		rtt += now.tv_usec;
		rtt -= request->proxy_when.tv_usec;

		if (!home->has_rtt) {
			home->has_rtt = TRUE;

			home->srtt = rtt;
			home->rttvar = rtt / 2;

		} else {
			home->rttvar -= home->rttvar >> 2;
			home->rttvar += (home->srtt - rtt);
			home->srtt -= home->srtt >> 3;
			home->srtt += rtt >> 3;
		}

		home->rto = home->srtt;
		if (home->rttvar > (USEC / 4)) {
			home->rto += home->rttvar * 4;
		} else {
			home->rto += USEC;
		}
	}