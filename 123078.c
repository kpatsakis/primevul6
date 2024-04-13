static int can_handle_new_request(RADIUS_PACKET *packet,
				  RADCLIENT *client,
				  struct main_config_t *root)
{
	/*
	 *	Count the total number of requests, to see if
	 *	there are too many.  If so, return with an
	 *	error.
	 */
	if (root->max_requests) {
		int request_count = fr_packet_list_num_elements(pl);

		/*
		 *	This is a new request.  Let's see if
		 *	it makes us go over our configured
		 *	bounds.
		 */
		if (request_count > root->max_requests) {
			radlog(L_ERR, "Dropping request (%d is too many): "
			       "from client %s port %d - ID: %d", request_count,
			       client->shortname,
			       packet->src_port, packet->id);
			radlog(L_INFO, "WARNING: Please check the configuration file.\n"
			       "\tThe value for 'max_requests' is probably set too low.\n");
			return 0;
		} /* else there were a small number of requests */
	} /* else there was no configured limit for requests */

	/*
	 *	FIXME: Add per-client checks.  If one client is sending
	 *	too many packets, start discarding them.
	 *
	 *	We increment the counters here, and decrement them
	 *	when the response is sent... somewhere in this file.
	 */

	/*
	 *	FUTURE: Add checks for system load.  If the system is
	 *	busy, start dropping requests...
	 *
	 *	We can probably keep some statistics ourselves...  if
	 *	there are more requests coming in than we can handle,
	 *	start dropping some.
	 */

	return 1;
}