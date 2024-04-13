static int __must_check rose_add_node(struct rose_route_struct *rose_route,
	struct net_device *dev)
{
	struct rose_node  *rose_node, *rose_tmpn, *rose_tmpp;
	struct rose_neigh *rose_neigh;
	int i, res = 0;

	spin_lock_bh(&rose_node_list_lock);
	spin_lock_bh(&rose_neigh_list_lock);

	rose_node = rose_node_list;
	while (rose_node != NULL) {
		if ((rose_node->mask == rose_route->mask) &&
		    (rosecmpm(&rose_route->address, &rose_node->address,
			      rose_route->mask) == 0))
			break;
		rose_node = rose_node->next;
	}

	if (rose_node != NULL && rose_node->loopback) {
		res = -EINVAL;
		goto out;
	}

	rose_neigh = rose_neigh_list;
	while (rose_neigh != NULL) {
		if (ax25cmp(&rose_route->neighbour,
			    &rose_neigh->callsign) == 0 &&
		    rose_neigh->dev == dev)
			break;
		rose_neigh = rose_neigh->next;
	}

	if (rose_neigh == NULL) {
		rose_neigh = kmalloc(sizeof(*rose_neigh), GFP_ATOMIC);
		if (rose_neigh == NULL) {
			res = -ENOMEM;
			goto out;
		}

		rose_neigh->callsign  = rose_route->neighbour;
		rose_neigh->digipeat  = NULL;
		rose_neigh->ax25      = NULL;
		rose_neigh->dev       = dev;
		rose_neigh->count     = 0;
		rose_neigh->use       = 0;
		rose_neigh->dce_mode  = 0;
		rose_neigh->loopback  = 0;
		rose_neigh->number    = rose_neigh_no++;
		rose_neigh->restarted = 0;

		skb_queue_head_init(&rose_neigh->queue);

		timer_setup(&rose_neigh->ftimer, NULL, 0);
		timer_setup(&rose_neigh->t0timer, NULL, 0);

		if (rose_route->ndigis != 0) {
			rose_neigh->digipeat =
				kmalloc(sizeof(ax25_digi), GFP_ATOMIC);
			if (rose_neigh->digipeat == NULL) {
				kfree(rose_neigh);
				res = -ENOMEM;
				goto out;
			}

			rose_neigh->digipeat->ndigi      = rose_route->ndigis;
			rose_neigh->digipeat->lastrepeat = -1;

			for (i = 0; i < rose_route->ndigis; i++) {
				rose_neigh->digipeat->calls[i]    =
					rose_route->digipeaters[i];
				rose_neigh->digipeat->repeated[i] = 0;
			}
		}

		rose_neigh->next = rose_neigh_list;
		rose_neigh_list  = rose_neigh;
	}

	/*
	 * This is a new node to be inserted into the list. Find where it needs
	 * to be inserted into the list, and insert it. We want to be sure
	 * to order the list in descending order of mask size to ensure that
	 * later when we are searching this list the first match will be the
	 * best match.
	 */
	if (rose_node == NULL) {
		rose_tmpn = rose_node_list;
		rose_tmpp = NULL;

		while (rose_tmpn != NULL) {
			if (rose_tmpn->mask > rose_route->mask) {
				rose_tmpp = rose_tmpn;
				rose_tmpn = rose_tmpn->next;
			} else {
				break;
			}
		}

		/* create new node */
		rose_node = kmalloc(sizeof(*rose_node), GFP_ATOMIC);
		if (rose_node == NULL) {
			res = -ENOMEM;
			goto out;
		}

		rose_node->address      = rose_route->address;
		rose_node->mask         = rose_route->mask;
		rose_node->count        = 1;
		rose_node->loopback     = 0;
		rose_node->neighbour[0] = rose_neigh;

		if (rose_tmpn == NULL) {
			if (rose_tmpp == NULL) {	/* Empty list */
				rose_node_list  = rose_node;
				rose_node->next = NULL;
			} else {
				rose_tmpp->next = rose_node;
				rose_node->next = NULL;
			}
		} else {
			if (rose_tmpp == NULL) {	/* 1st node */
				rose_node->next = rose_node_list;
				rose_node_list  = rose_node;
			} else {
				rose_tmpp->next = rose_node;
				rose_node->next = rose_tmpn;
			}
		}
		rose_neigh->count++;

		goto out;
	}

	/* We have space, slot it in */
	if (rose_node->count < 3) {
		rose_node->neighbour[rose_node->count] = rose_neigh;
		rose_node->count++;
		rose_neigh->count++;
	}

out:
	spin_unlock_bh(&rose_neigh_list_lock);
	spin_unlock_bh(&rose_node_list_lock);

	return res;
}