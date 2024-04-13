static bool FNAME(is_rsvd_bits_set)(struct kvm_mmu *mmu, u64 gpte, int level)
{
	return __is_rsvd_bits_set(&mmu->guest_rsvd_check, gpte, level) ||
	       FNAME(is_bad_mt_xwr)(&mmu->guest_rsvd_check, gpte);
}