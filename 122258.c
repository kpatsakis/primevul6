static inline pmd_t pmd_read_atomic(pmd_t *pmdp)
{
	/*
	 * Depend on compiler for an atomic pmd read. NOTE: this is
	 * only going to work, if the pmdval_t isn't larger than
	 * an unsigned long.
	 */
	return *pmdp;
}