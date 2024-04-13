int rad_send(RADIUS_PACKET *packet, const RADIUS_PACKET *original,
	     const char *secret)
{
	VALUE_PAIR		*reply;
	const char		*what;
	char			ip_buffer[128];
	struct	sockaddr_in	saremote;
	struct	sockaddr_in	*sa;

	/*
	 *	Maybe it's a fake packet.  Don't send it.
	 */
	if (!packet || (packet->sockfd < 0)) {
		return 0;
	}

	if ((packet->code > 0) && (packet->code < 52)) {
		what = packet_codes[packet->code];
	} else {
		what = "Reply";
	}

	/*
	 *  First time through, allocate room for the packet
	 */
	if (!packet->data) {
		DEBUG("Sending %s of id %d to %s port %d\n",
		      what, packet->id,
		      ip_ntoa(ip_buffer, packet->dst_ipaddr),
		      packet->dst_port);
		
		/*
		 *	Encode the packet.
		 */
		if (rad_encode(packet, original, secret) < 0) {
			return -1;
		}
		
		/*
		 *	Re-sign it, including updating the
		 *	Message-Authenticator.
		 */
		if (rad_sign(packet, original, secret) < 0) {
			return -1;
		}

		/*
		 *	If packet->data points to data, then we print out
		 *	the VP list again only for debugging.
		 */
	} else if (librad_debug) {
	  	DEBUG("Re-sending %s of id %d to %s port %d\n", what, packet->id,
		      ip_ntoa(ip_buffer, packet->dst_ipaddr),
		      packet->dst_port);

		for (reply = packet->vps; reply; reply = reply->next) {
			/* FIXME: ignore attributes > 0xff */
			debug_pair(reply);
		}
	}

	/*
	 *	And send it on it's way.
	 */
	sa = (struct sockaddr_in *) &saremote;
        memset ((char *) sa, '\0', sizeof (saremote));
	sa->sin_family = AF_INET;
	sa->sin_addr.s_addr = packet->dst_ipaddr;
	sa->sin_port = htons(packet->dst_port);
#ifndef WITH_UDPFROMTO
	return sendto(packet->sockfd, packet->data, (int)packet->data_len, 0,
		      (struct sockaddr *)&saremote, sizeof(struct sockaddr_in));
#else
	{
		struct sockaddr_in salocal;
		memset ((char *) &salocal, '\0', sizeof (salocal));
		salocal.sin_family = AF_INET;
		salocal.sin_addr.s_addr = packet->src_ipaddr;
		
		return sendfromto(packet->sockfd, packet->data, (int)packet->data_len, 0,
				  (struct sockaddr *)&salocal,  sizeof(struct sockaddr_in),
				  (struct sockaddr *)&saremote, sizeof(struct sockaddr_in));
	}
#endif
}