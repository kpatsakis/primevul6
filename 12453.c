static void FNAME(pte_prefetch)(struct kvm_vcpu *vcpu, struct guest_walker *gw,
				u64 *sptep)
{
	struct kvm_mmu_page *sp;
	pt_element_t *gptep = gw->prefetch_ptes;
	u64 *spte;
	int i;

	sp = sptep_to_sp(sptep);

	if (sp->role.level > PG_LEVEL_4K)
		return;

	/*
	 * If addresses are being invalidated, skip prefetching to avoid
	 * accidentally prefetching those addresses.
	 */
	if (unlikely(vcpu->kvm->mmu_notifier_count))
		return;

	if (sp->role.direct)
		return __direct_pte_prefetch(vcpu, sp, sptep);

	i = (sptep - sp->spt) & ~(PTE_PREFETCH_NUM - 1);
	spte = sp->spt + i;

	for (i = 0; i < PTE_PREFETCH_NUM; i++, spte++) {
		if (spte == sptep)
			continue;

		if (is_shadow_present_pte(*spte))
			continue;

		if (!FNAME(prefetch_gpte)(vcpu, sp, spte, gptep[i], true))
			break;
	}
}