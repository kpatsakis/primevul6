static void prep_new_huge_page(struct hstate *h, struct page *page, int nid)
{
	__prep_new_huge_page(h, page);
	spin_lock_irq(&hugetlb_lock);
	__prep_account_new_huge_page(h, nid);
	spin_unlock_irq(&hugetlb_lock);
}