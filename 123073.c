static void debug_packet(REQUEST *request, RADIUS_PACKET *packet, int direction)
{
	VALUE_PAIR *vp;
	char buffer[1024];
	const char *received, *from;
	const fr_ipaddr_t *ip;
	int port;

	if (!packet) return;

	rad_assert(request->radlog != NULL);

	if (direction == 0) {
		received = "Received";
		from = "from";	/* what else? */
		ip = &packet->src_ipaddr;
		port = packet->src_port;

	} else {
		received = "Sending";
		from = "to";	/* hah! */
		ip = &packet->dst_ipaddr;
		port = packet->dst_port;
	}
	
	/*
	 *	Client-specific debugging re-prints the input
	 *	packet into the client log.
	 *
	 *	This really belongs in a utility library
	 */
	if ((packet->code > 0) && (packet->code < FR_MAX_PACKET_CODE)) {
		RDEBUG("%s %s packet %s host %s port %d, id=%d, length=%d",
		       received, fr_packet_codes[packet->code], from,
		       inet_ntop(ip->af, &ip->ipaddr, buffer, sizeof(buffer)),
		       port, packet->id, packet->data_len);
	} else {
		RDEBUG("%s packet %s host %s port %d code=%d, id=%d, length=%d",
		       received, from,
		       inet_ntop(ip->af, &ip->ipaddr, buffer, sizeof(buffer)),
		       port,
		       packet->code, packet->id, packet->data_len);
	}

	for (vp = packet->vps; vp != NULL; vp = vp->next) {
		vp_prints(buffer, sizeof(buffer), vp);
		request->radlog(L_DBG, 0, request, "\t%s", buffer);
	}
}