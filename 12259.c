int rose_add_loopback_node(const rose_address *address)
{
	struct rose_node *rose_node;
	int err = 0;

	spin_lock_bh(&rose_node_list_lock);

	rose_node = rose_node_list;
	while (rose_node != NULL) {
		if ((rose_node->mask == 10) &&
		     (rosecmpm(address, &rose_node->address, 10) == 0) &&
		     rose_node->loopback)
			break;
		rose_node = rose_node->next;
	}

	if (rose_node != NULL)
		goto out;

	if ((rose_node = kmalloc(sizeof(*rose_node), GFP_ATOMIC)) == NULL) {
		err = -ENOMEM;
		goto out;
	}

	rose_node->address      = *address;
	rose_node->mask         = 10;
	rose_node->count        = 1;
	rose_node->loopback     = 1;
	rose_node->neighbour[0] = rose_loopback_neigh;

	/* Insert at the head of list. Address is always mask=10 */
	rose_node->next = rose_node_list;
	rose_node_list  = rose_node;

	rose_loopback_neigh->count++;

out:
	spin_unlock_bh(&rose_node_list_lock);

	return err;
}