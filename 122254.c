static inline pmd_t pmd_read_atomic(pmd_t *pmdp)
{
	return (pmd_t) { atomic64_read((atomic64_t *)pmdp) };
}