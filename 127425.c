static int cvt_gate_to_trap(int vector, const gate_desc *val,
			    struct trap_info *info)
{
	unsigned long addr;

	if (val->bits.type != GATE_TRAP && val->bits.type != GATE_INTERRUPT)
		return 0;

	info->vector = vector;

	addr = gate_offset(val);
#ifdef CONFIG_X86_64
	if (!get_trap_addr((void **)&addr, val->bits.ist))
		return 0;
#endif	/* CONFIG_X86_64 */
	info->address = addr;

	info->cs = gate_segment(val);
	info->flags = val->bits.dpl;
	/* interrupt gates clear IF */
	if (val->bits.type == GATE_INTERRUPT)
		info->flags |= 1 << 2;

	return 1;
}