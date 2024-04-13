void putback_active_hugepage(struct page *page)
{
	spin_lock_irq(&hugetlb_lock);
	SetHPageMigratable(page);
	list_move_tail(&page->lru, &(page_hstate(page))->hugepage_activelist);
	spin_unlock_irq(&hugetlb_lock);
	put_page(page);
}