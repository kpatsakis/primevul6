int rose_route_frame(struct sk_buff *skb, ax25_cb *ax25)
{
	struct rose_neigh *rose_neigh, *new_neigh;
	struct rose_route *rose_route;
	struct rose_facilities_struct facilities;
	rose_address *src_addr, *dest_addr;
	struct sock *sk;
	unsigned short frametype;
	unsigned int lci, new_lci;
	unsigned char cause, diagnostic;
	struct net_device *dev;
	int res = 0;
	char buf[11];

	if (skb->len < ROSE_MIN_LEN)
		return res;

	if (!ax25)
		return rose_loopback_queue(skb, NULL);

	frametype = skb->data[2];
	lci = ((skb->data[0] << 8) & 0xF00) + ((skb->data[1] << 0) & 0x0FF);
	if (frametype == ROSE_CALL_REQUEST &&
	    (skb->len <= ROSE_CALL_REQ_FACILITIES_OFF ||
	     skb->data[ROSE_CALL_REQ_ADDR_LEN_OFF] !=
	     ROSE_CALL_REQ_ADDR_LEN_VAL))
		return res;
	src_addr  = (rose_address *)(skb->data + ROSE_CALL_REQ_SRC_ADDR_OFF);
	dest_addr = (rose_address *)(skb->data + ROSE_CALL_REQ_DEST_ADDR_OFF);

	spin_lock_bh(&rose_neigh_list_lock);
	spin_lock_bh(&rose_route_list_lock);

	rose_neigh = rose_neigh_list;
	while (rose_neigh != NULL) {
		if (ax25cmp(&ax25->dest_addr, &rose_neigh->callsign) == 0 &&
		    ax25->ax25_dev->dev == rose_neigh->dev)
			break;
		rose_neigh = rose_neigh->next;
	}

	if (rose_neigh == NULL) {
		printk("rose_route : unknown neighbour or device %s\n",
		       ax2asc(buf, &ax25->dest_addr));
		goto out;
	}

	/*
	 *	Obviously the link is working, halt the ftimer.
	 */
	rose_stop_ftimer(rose_neigh);

	/*
	 *	LCI of zero is always for us, and its always a restart
	 * 	frame.
	 */
	if (lci == 0) {
		rose_link_rx_restart(skb, rose_neigh, frametype);
		goto out;
	}

	/*
	 *	Find an existing socket.
	 */
	if ((sk = rose_find_socket(lci, rose_neigh)) != NULL) {
		if (frametype == ROSE_CALL_REQUEST) {
			struct rose_sock *rose = rose_sk(sk);

			/* Remove an existing unused socket */
			rose_clear_queues(sk);
			rose->cause	 = ROSE_NETWORK_CONGESTION;
			rose->diagnostic = 0;
			rose->neighbour->use--;
			rose->neighbour	 = NULL;
			rose->lci	 = 0;
			rose->state	 = ROSE_STATE_0;
			sk->sk_state	 = TCP_CLOSE;
			sk->sk_err	 = 0;
			sk->sk_shutdown	 |= SEND_SHUTDOWN;
			if (!sock_flag(sk, SOCK_DEAD)) {
				sk->sk_state_change(sk);
				sock_set_flag(sk, SOCK_DEAD);
			}
		}
		else {
			skb_reset_transport_header(skb);
			res = rose_process_rx_frame(sk, skb);
			goto out;
		}
	}

	/*
	 *	Is is a Call Request and is it for us ?
	 */
	if (frametype == ROSE_CALL_REQUEST)
		if ((dev = rose_dev_get(dest_addr)) != NULL) {
			res = rose_rx_call_request(skb, dev, rose_neigh, lci);
			dev_put(dev);
			goto out;
		}

	if (!sysctl_rose_routing_control) {
		rose_transmit_clear_request(rose_neigh, lci, ROSE_NOT_OBTAINABLE, 0);
		goto out;
	}

	/*
	 *	Route it to the next in line if we have an entry for it.
	 */
	rose_route = rose_route_list;
	while (rose_route != NULL) {
		if (rose_route->lci1 == lci &&
		    rose_route->neigh1 == rose_neigh) {
			if (frametype == ROSE_CALL_REQUEST) {
				/* F6FBB - Remove an existing unused route */
				rose_remove_route(rose_route);
				break;
			} else if (rose_route->neigh2 != NULL) {
				skb->data[0] &= 0xF0;
				skb->data[0] |= (rose_route->lci2 >> 8) & 0x0F;
				skb->data[1]  = (rose_route->lci2 >> 0) & 0xFF;
				rose_transmit_link(skb, rose_route->neigh2);
				if (frametype == ROSE_CLEAR_CONFIRMATION)
					rose_remove_route(rose_route);
				res = 1;
				goto out;
			} else {
				if (frametype == ROSE_CLEAR_CONFIRMATION)
					rose_remove_route(rose_route);
				goto out;
			}
		}
		if (rose_route->lci2 == lci &&
		    rose_route->neigh2 == rose_neigh) {
			if (frametype == ROSE_CALL_REQUEST) {
				/* F6FBB - Remove an existing unused route */
				rose_remove_route(rose_route);
				break;
			} else if (rose_route->neigh1 != NULL) {
				skb->data[0] &= 0xF0;
				skb->data[0] |= (rose_route->lci1 >> 8) & 0x0F;
				skb->data[1]  = (rose_route->lci1 >> 0) & 0xFF;
				rose_transmit_link(skb, rose_route->neigh1);
				if (frametype == ROSE_CLEAR_CONFIRMATION)
					rose_remove_route(rose_route);
				res = 1;
				goto out;
			} else {
				if (frametype == ROSE_CLEAR_CONFIRMATION)
					rose_remove_route(rose_route);
				goto out;
			}
		}
		rose_route = rose_route->next;
	}

	/*
	 *	We know that:
	 *	1. The frame isn't for us,
	 *	2. It isn't "owned" by any existing route.
	 */
	if (frametype != ROSE_CALL_REQUEST) {	/* XXX */
		res = 0;
		goto out;
	}

	memset(&facilities, 0x00, sizeof(struct rose_facilities_struct));

	if (!rose_parse_facilities(skb->data + ROSE_CALL_REQ_FACILITIES_OFF,
				   skb->len - ROSE_CALL_REQ_FACILITIES_OFF,
				   &facilities)) {
		rose_transmit_clear_request(rose_neigh, lci, ROSE_INVALID_FACILITY, 76);
		goto out;
	}

	/*
	 *	Check for routing loops.
	 */
	rose_route = rose_route_list;
	while (rose_route != NULL) {
		if (rose_route->rand == facilities.rand &&
		    rosecmp(src_addr, &rose_route->src_addr) == 0 &&
		    ax25cmp(&facilities.dest_call, &rose_route->src_call) == 0 &&
		    ax25cmp(&facilities.source_call, &rose_route->dest_call) == 0) {
			rose_transmit_clear_request(rose_neigh, lci, ROSE_NOT_OBTAINABLE, 120);
			goto out;
		}
		rose_route = rose_route->next;
	}

	if ((new_neigh = rose_get_neigh(dest_addr, &cause, &diagnostic, 1)) == NULL) {
		rose_transmit_clear_request(rose_neigh, lci, cause, diagnostic);
		goto out;
	}

	if ((new_lci = rose_new_lci(new_neigh)) == 0) {
		rose_transmit_clear_request(rose_neigh, lci, ROSE_NETWORK_CONGESTION, 71);
		goto out;
	}

	if ((rose_route = kmalloc(sizeof(*rose_route), GFP_ATOMIC)) == NULL) {
		rose_transmit_clear_request(rose_neigh, lci, ROSE_NETWORK_CONGESTION, 120);
		goto out;
	}

	rose_route->lci1      = lci;
	rose_route->src_addr  = *src_addr;
	rose_route->dest_addr = *dest_addr;
	rose_route->src_call  = facilities.dest_call;
	rose_route->dest_call = facilities.source_call;
	rose_route->rand      = facilities.rand;
	rose_route->neigh1    = rose_neigh;
	rose_route->lci2      = new_lci;
	rose_route->neigh2    = new_neigh;

	rose_route->neigh1->use++;
	rose_route->neigh2->use++;

	rose_route->next = rose_route_list;
	rose_route_list  = rose_route;

	skb->data[0] &= 0xF0;
	skb->data[0] |= (rose_route->lci2 >> 8) & 0x0F;
	skb->data[1]  = (rose_route->lci2 >> 0) & 0xFF;

	rose_transmit_link(skb, rose_route->neigh2);
	res = 1;

out:
	spin_unlock_bh(&rose_route_list_lock);
	spin_unlock_bh(&rose_neigh_list_lock);

	return res;
}