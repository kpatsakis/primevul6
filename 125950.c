void move_hugetlb_state(struct page *oldpage, struct page *newpage, int reason)
{
	struct hstate *h = page_hstate(oldpage);

	hugetlb_cgroup_migrate(oldpage, newpage);
	set_page_owner_migrate_reason(newpage, reason);

	/*
	 * transfer temporary state of the new huge page. This is
	 * reverse to other transitions because the newpage is going to
	 * be final while the old one will be freed so it takes over
	 * the temporary status.
	 *
	 * Also note that we have to transfer the per-node surplus state
	 * here as well otherwise the global surplus count will not match
	 * the per-node's.
	 */
	if (HPageTemporary(newpage)) {
		int old_nid = page_to_nid(oldpage);
		int new_nid = page_to_nid(newpage);

		SetHPageTemporary(oldpage);
		ClearHPageTemporary(newpage);

		/*
		 * There is no need to transfer the per-node surplus state
		 * when we do not cross the node.
		 */
		if (new_nid == old_nid)
			return;
		spin_lock_irq(&hugetlb_lock);
		if (h->surplus_huge_pages_node[old_nid]) {
			h->surplus_huge_pages_node[old_nid]--;
			h->surplus_huge_pages_node[new_nid]++;
		}
		spin_unlock_irq(&hugetlb_lock);
	}
}