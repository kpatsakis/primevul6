static struct page *pmd_to_page(pmd_t *pmd)
{
	unsigned long mask = ~(PTRS_PER_PMD * sizeof(pmd_t) - 1);
	return virt_to_page((void *)((unsigned long) pmd & mask));
}