static inline void native_set_pte_atomic(pte_t *ptep, pte_t pte)
{
	set_64bit((unsigned long long *)(ptep), native_pte_val(pte));
}