static int rose_del_node(struct rose_route_struct *rose_route,
	struct net_device *dev)
{
	struct rose_node  *rose_node;
	struct rose_neigh *rose_neigh;
	int i, err = 0;

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

	if (rose_node == NULL || rose_node->loopback) {
		err = -EINVAL;
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
		err = -EINVAL;
		goto out;
	}

	for (i = 0; i < rose_node->count; i++) {
		if (rose_node->neighbour[i] == rose_neigh) {
			rose_neigh->count--;

			if (rose_neigh->count == 0 && rose_neigh->use == 0)
				rose_remove_neigh(rose_neigh);

			rose_node->count--;

			if (rose_node->count == 0) {
				rose_remove_node(rose_node);
			} else {
				switch (i) {
				case 0:
					rose_node->neighbour[0] =
						rose_node->neighbour[1];
					fallthrough;
				case 1:
					rose_node->neighbour[1] =
						rose_node->neighbour[2];
					break;
				case 2:
					break;
				}
			}
			goto out;
		}
	}
	err = -EINVAL;

out:
	spin_unlock_bh(&rose_neigh_list_lock);
	spin_unlock_bh(&rose_node_list_lock);

	return err;
}