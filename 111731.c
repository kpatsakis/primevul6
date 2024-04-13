static void _handle_arm_thumb(struct MACH0_(obj_t) *bin, RBinSymbol **p) {
	RBinSymbol *ptr = *p;
	if (bin) {
		if (ptr->paddr & 1) {
			ptr->paddr--;
			ptr->vaddr--;
			ptr->bits = 16;
		}
	}
}