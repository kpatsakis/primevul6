static inline pmd_t native_pmdp_get_and_clear(pmd_t *pmdp)
{
	union split_pmd res, *orig = (union split_pmd *)pmdp;

	/* xchg acts as a barrier before setting of the high bits */
	res.pmd_low = xchg(&orig->pmd_low, 0);
	res.pmd_high = orig->pmd_high;
	orig->pmd_high = 0;

	return res.pmd;
}