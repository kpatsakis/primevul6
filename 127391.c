static inline void __set_fixmap(unsigned /* enum fixed_addresses */ idx,
				phys_addr_t phys, pgprot_t flags)
{
	pv_ops.mmu.set_fixmap(idx, phys, flags);
}