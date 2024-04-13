static void __prep_account_new_huge_page(struct hstate *h, int nid)
{
	lockdep_assert_held(&hugetlb_lock);
	h->nr_huge_pages++;
	h->nr_huge_pages_node[nid]++;
}