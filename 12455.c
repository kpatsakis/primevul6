static inline void FNAME(protect_clean_gpte)(struct kvm_mmu *mmu, unsigned *access,
					     unsigned gpte)
{
	unsigned mask;

	/* dirty bit is not supported, so no need to track it */
	if (!PT_HAVE_ACCESSED_DIRTY(mmu))
		return;

	BUILD_BUG_ON(PT_WRITABLE_MASK != ACC_WRITE_MASK);

	mask = (unsigned)~ACC_WRITE_MASK;
	/* Allow write access to dirty gptes */
	mask |= (gpte >> (PT_GUEST_DIRTY_SHIFT - PT_WRITABLE_SHIFT)) &
		PT_WRITABLE_MASK;
	*access &= mask;
}