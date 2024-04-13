static ssize_t demote_store(struct kobject *kobj,
	       struct kobj_attribute *attr, const char *buf, size_t len)
{
	unsigned long nr_demote;
	unsigned long nr_available;
	nodemask_t nodes_allowed, *n_mask;
	struct hstate *h;
	int err = 0;
	int nid;

	err = kstrtoul(buf, 10, &nr_demote);
	if (err)
		return err;
	h = kobj_to_hstate(kobj, &nid);

	if (nid != NUMA_NO_NODE) {
		init_nodemask_of_node(&nodes_allowed, nid);
		n_mask = &nodes_allowed;
	} else {
		n_mask = &node_states[N_MEMORY];
	}

	/* Synchronize with other sysfs operations modifying huge pages */
	mutex_lock(&h->resize_lock);
	spin_lock_irq(&hugetlb_lock);

	while (nr_demote) {
		/*
		 * Check for available pages to demote each time thorough the
		 * loop as demote_pool_huge_page will drop hugetlb_lock.
		 */
		if (nid != NUMA_NO_NODE)
			nr_available = h->free_huge_pages_node[nid];
		else
			nr_available = h->free_huge_pages;
		nr_available -= h->resv_huge_pages;
		if (!nr_available)
			break;

		err = demote_pool_huge_page(h, n_mask);
		if (err)
			break;

		nr_demote--;
	}

	spin_unlock_irq(&hugetlb_lock);
	mutex_unlock(&h->resize_lock);

	if (err)
		return err;
	return len;
}