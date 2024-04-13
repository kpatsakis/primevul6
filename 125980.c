int get_hwpoison_huge_page(struct page *page, bool *hugetlb)
{
	int ret = 0;

	*hugetlb = false;
	spin_lock_irq(&hugetlb_lock);
	if (PageHeadHuge(page)) {
		*hugetlb = true;
		if (HPageFreed(page) || HPageMigratable(page))
			ret = get_page_unless_zero(page);
		else
			ret = -EBUSY;
	}
	spin_unlock_irq(&hugetlb_lock);
	return ret;
}