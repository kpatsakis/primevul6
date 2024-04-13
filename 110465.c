static int FNAME(cmpxchg_gpte)(struct kvm_vcpu *vcpu, struct kvm_mmu *mmu,
			       pt_element_t __user *ptep_user, unsigned index,
			       pt_element_t orig_pte, pt_element_t new_pte)
{
	int npages;
	pt_element_t ret;
	pt_element_t *table;
	struct page *page;

	npages = get_user_pages_fast((unsigned long)ptep_user, 1, 1, &page);
	/* Check if the user is doing something meaningless. */
	if (unlikely(npages != 1))
		return -EFAULT;

	table = kmap_atomic(page);
	ret = CMPXCHG(&table[index], orig_pte, new_pte);
	kunmap_atomic(table);

	kvm_release_page_dirty(page);

	return (ret != orig_pte);
}