static inline void native_set_pud(pud_t *pudp, pud_t pud)
{
	set_64bit((unsigned long long *)(pudp), native_pud_val(pud));
}