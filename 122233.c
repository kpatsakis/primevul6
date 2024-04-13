static inline void native_set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	set_64bit((unsigned long long *)(pmdp), native_pmd_val(pmd));
}