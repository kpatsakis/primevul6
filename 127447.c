static inline void set_pte_atomic(pte_t *ptep, pte_t pte)
{
	set_pte(ptep, pte);
}