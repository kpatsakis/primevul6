void rose_del_loopback_node(const rose_address *address)
{
	struct rose_node *rose_node;

	spin_lock_bh(&rose_node_list_lock);

	rose_node = rose_node_list;
	while (rose_node != NULL) {
		if ((rose_node->mask == 10) &&
		    (rosecmpm(address, &rose_node->address, 10) == 0) &&
		    rose_node->loopback)
			break;
		rose_node = rose_node->next;
	}

	if (rose_node == NULL)
		goto out;

	rose_remove_node(rose_node);

	rose_loopback_neigh->count--;

out:
	spin_unlock_bh(&rose_node_list_lock);
}