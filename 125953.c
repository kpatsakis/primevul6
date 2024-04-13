bool want_pmd_share(struct vm_area_struct *vma, unsigned long addr)
{
#ifdef CONFIG_USERFAULTFD
	if (uffd_disable_huge_pmd_share(vma))
		return false;
#endif
	return vma_shareable(vma, addr);
}