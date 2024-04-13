static unsigned int allowed_mems_nr(struct hstate *h)
{
	int node;
	unsigned int nr = 0;
	nodemask_t *mpol_allowed;
	unsigned int *array = h->free_huge_pages_node;
	gfp_t gfp_mask = htlb_alloc_mask(h);

	mpol_allowed = policy_nodemask_current(gfp_mask);

	for_each_node_mask(node, cpuset_current_mems_allowed) {
		if (!mpol_allowed || node_isset(node, *mpol_allowed))
			nr += array[node];
	}

	return nr;
}