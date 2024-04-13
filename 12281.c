static void rose_del_route_by_neigh(struct rose_neigh *rose_neigh)
{
	struct rose_route *rose_route, *s;

	rose_neigh->restarted = 0;

	rose_stop_t0timer(rose_neigh);
	rose_start_ftimer(rose_neigh);

	skb_queue_purge(&rose_neigh->queue);

	spin_lock_bh(&rose_route_list_lock);

	rose_route = rose_route_list;

	while (rose_route != NULL) {
		if ((rose_route->neigh1 == rose_neigh && rose_route->neigh2 == rose_neigh) ||
		    (rose_route->neigh1 == rose_neigh && rose_route->neigh2 == NULL)       ||
		    (rose_route->neigh2 == rose_neigh && rose_route->neigh1 == NULL)) {
			s = rose_route->next;
			rose_remove_route(rose_route);
			rose_route = s;
			continue;
		}

		if (rose_route->neigh1 == rose_neigh) {
			rose_route->neigh1->use--;
			rose_route->neigh1 = NULL;
			rose_transmit_clear_request(rose_route->neigh2, rose_route->lci2, ROSE_OUT_OF_ORDER, 0);
		}

		if (rose_route->neigh2 == rose_neigh) {
			rose_route->neigh2->use--;
			rose_route->neigh2 = NULL;
			rose_transmit_clear_request(rose_route->neigh1, rose_route->lci1, ROSE_OUT_OF_ORDER, 0);
		}

		rose_route = rose_route->next;
	}
	spin_unlock_bh(&rose_route_list_lock);
}