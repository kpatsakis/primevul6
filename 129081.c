static inline void elf_core_copy_regs(elf_gregset_t *elfregs, struct pt_regs *regs)
{
	ELF_CORE_COPY_REGS((*elfregs), regs)
}