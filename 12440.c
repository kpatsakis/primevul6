static inline unsigned FNAME(gpte_pkeys)(struct kvm_vcpu *vcpu, u64 gpte)
{
	unsigned pkeys = 0;
#if PTTYPE == 64
	pte_t pte = {.pte = gpte};

	pkeys = pte_flags_pkey(pte_flags(pte));
#endif
	return pkeys;
}