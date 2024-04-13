struct rose_neigh *rose_get_neigh(rose_address *addr, unsigned char *cause,
	unsigned char *diagnostic, int route_frame)
{
	struct rose_neigh *res = NULL;
	struct rose_node *node;
	int failed = 0;
	int i;

	if (!route_frame) spin_lock_bh(&rose_node_list_lock);
	for (node = rose_node_list; node != NULL; node = node->next) {
		if (rosecmpm(addr, &node->address, node->mask) == 0) {
			for (i = 0; i < node->count; i++) {
				if (node->neighbour[i]->restarted) {
					res = node->neighbour[i];
					goto out;
				}
			}
		}
	}
	if (!route_frame) { /* connect request */
		for (node = rose_node_list; node != NULL; node = node->next) {
			if (rosecmpm(addr, &node->address, node->mask) == 0) {
				for (i = 0; i < node->count; i++) {
					if (!rose_ftimer_running(node->neighbour[i])) {
						res = node->neighbour[i];
						goto out;
					}
					failed = 1;
				}
			}
		}
	}

	if (failed) {
		*cause      = ROSE_OUT_OF_ORDER;
		*diagnostic = 0;
	} else {
		*cause      = ROSE_NOT_OBTAINABLE;
		*diagnostic = 0;
	}

out:
	if (!route_frame) spin_unlock_bh(&rose_node_list_lock);
	return res;
}