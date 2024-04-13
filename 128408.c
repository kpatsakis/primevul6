void sctp_assoc_rm_peer(struct sctp_association *asoc,
			struct sctp_transport *peer)
{
	struct list_head	*pos;
	struct sctp_transport	*transport;

	pr_debug("%s: association:%p addr:%pISpc\n",
		 __func__, asoc, &peer->ipaddr.sa);

	/* If we are to remove the current retran_path, update it
	 * to the next peer before removing this peer from the list.
	 */
	if (asoc->peer.retran_path == peer)
		sctp_assoc_update_retran_path(asoc);

	/* Remove this peer from the list. */
	list_del_rcu(&peer->transports);

	/* Get the first transport of asoc. */
	pos = asoc->peer.transport_addr_list.next;
	transport = list_entry(pos, struct sctp_transport, transports);

	/* Update any entries that match the peer to be deleted. */
	if (asoc->peer.primary_path == peer)
		sctp_assoc_set_primary(asoc, transport);
	if (asoc->peer.active_path == peer)
		asoc->peer.active_path = transport;
	if (asoc->peer.retran_path == peer)
		asoc->peer.retran_path = transport;
	if (asoc->peer.last_data_from == peer)
		asoc->peer.last_data_from = transport;

	/* If we remove the transport an INIT was last sent to, set it to
	 * NULL. Combined with the update of the retran path above, this
	 * will cause the next INIT to be sent to the next available
	 * transport, maintaining the cycle.
	 */
	if (asoc->init_last_sent_to == peer)
		asoc->init_last_sent_to = NULL;

	/* If we remove the transport an SHUTDOWN was last sent to, set it
	 * to NULL. Combined with the update of the retran path above, this
	 * will cause the next SHUTDOWN to be sent to the next available
	 * transport, maintaining the cycle.
	 */
	if (asoc->shutdown_last_sent_to == peer)
		asoc->shutdown_last_sent_to = NULL;

	/* If we remove the transport an ASCONF was last sent to, set it to
	 * NULL.
	 */
	if (asoc->addip_last_asconf &&
	    asoc->addip_last_asconf->transport == peer)
		asoc->addip_last_asconf->transport = NULL;

	/* If we have something on the transmitted list, we have to
	 * save it off.  The best place is the active path.
	 */
	if (!list_empty(&peer->transmitted)) {
		struct sctp_transport *active = asoc->peer.active_path;
		struct sctp_chunk *ch;

		/* Reset the transport of each chunk on this list */
		list_for_each_entry(ch, &peer->transmitted,
					transmitted_list) {
			ch->transport = NULL;
			ch->rtt_in_progress = 0;
		}

		list_splice_tail_init(&peer->transmitted,
					&active->transmitted);

		/* Start a T3 timer here in case it wasn't running so
		 * that these migrated packets have a chance to get
		 * retransmitted.
		 */
		if (!timer_pending(&active->T3_rtx_timer))
			if (!mod_timer(&active->T3_rtx_timer,
					jiffies + active->rto))
				sctp_transport_hold(active);
	}

	asoc->peer.transport_count--;

	sctp_transport_free(peer);
}