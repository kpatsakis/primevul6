static inline void pud_clear(pud_t *pudp)
{
	set_pud(pudp, __pud(0));
}