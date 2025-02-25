pci_emul_alloc_pbar(struct pci_vdev *pdi, int idx, uint64_t hostbase,
		    enum pcibar_type type, uint64_t size)
{
	int error;
	uint64_t *baseptr, limit, addr, mask, lobits, bar;

	assert(idx >= 0 && idx <= PCI_BARMAX);

	if ((size & (size - 1)) != 0)
		size = 1UL << flsl(size);	/* round up to a power of 2 */

	/* Enforce minimum BAR sizes required by the PCI standard */
	if (type == PCIBAR_IO) {
		if (size < 4)
			size = 4;
	} else {
		if (size < 16)
			size = 16;
	}

	switch (type) {
	case PCIBAR_NONE:
		baseptr = NULL;
		addr = mask = lobits = 0;
		break;
	case PCIBAR_IO:
		baseptr = &pci_emul_iobase;
		limit = PCI_EMUL_IOLIMIT;
		mask = PCIM_BAR_IO_BASE;
		lobits = PCIM_BAR_IO_SPACE;
		break;
	case PCIBAR_MEM64:
		/*
		 * FIXME
		 * Some drivers do not work well if the 64-bit BAR is allocated
		 * above 4GB. Allow for this by allocating small requests under
		 * 4GB unless then allocation size is larger than some arbitrary
		 * number (32MB currently). If guest booted by ovmf, then skip the
		 * workaround.
		 */
		if (!skip_pci_mem64bar_workaround && (size <= 32 * 1024 * 1024)) {
			baseptr = &pci_emul_membase32;
			limit = PCI_EMUL_MEMLIMIT32;
			mask = PCIM_BAR_MEM_BASE;
			lobits = PCIM_BAR_MEM_SPACE | PCIM_BAR_MEM_64;
			break;
		}

		/*
		 * XXX special case for device requiring peer-peer DMA
		 */
		if (size == 0x100000000UL)
			baseptr = &hostbase;
		else
			baseptr = &pci_emul_membase64;
		limit = PCI_EMUL_MEMLIMIT64;
		mask = PCIM_BAR_MEM_BASE;
		lobits = PCIM_BAR_MEM_SPACE | PCIM_BAR_MEM_64 |
			PCIM_BAR_MEM_PREFETCH;
		break;
	case PCIBAR_MEM32:
		baseptr = &pci_emul_membase32;
		limit = PCI_EMUL_MEMLIMIT32;
		mask = PCIM_BAR_MEM_BASE;
		lobits = PCIM_BAR_MEM_SPACE | PCIM_BAR_MEM_32;
		break;
	default:
		printf("pci_emul_alloc_base: invalid bar type %d\n", type);
		assert(0);
	}

	if (baseptr != NULL) {
		error = pci_emul_alloc_resource(baseptr, limit, size, &addr);
		if (error != 0)
			return error;
	}

	pdi->bar[idx].type = type;
	pdi->bar[idx].addr = addr;
	pdi->bar[idx].size = size;

	/* Initialize the BAR register in config space */
	bar = (addr & mask) | lobits;
	pci_set_cfgdata32(pdi, PCIR_BAR(idx), bar);

	if (type == PCIBAR_MEM64) {
		assert(idx + 1 <= PCI_BARMAX);
		pdi->bar[idx + 1].type = PCIBAR_MEMHI64;
		pci_set_cfgdata32(pdi, PCIR_BAR(idx + 1), bar >> 32);
	}

	register_bar(pdi, idx);

	return 0;
}