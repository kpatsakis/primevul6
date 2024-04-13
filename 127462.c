static bool __ref get_trap_addr(void **addr, unsigned int ist)
{
	unsigned int nr;
	bool ist_okay = false;

	/*
	 * Replace trap handler addresses by Xen specific ones.
	 * Check for known traps using IST and whitelist them.
	 * The debugger ones are the only ones we care about.
	 * Xen will handle faults like double_fault, so we should never see
	 * them.  Warn if there's an unexpected IST-using fault handler.
	 */
	for (nr = 0; nr < ARRAY_SIZE(trap_array); nr++) {
		struct trap_array_entry *entry = trap_array + nr;

		if (*addr == entry->orig) {
			*addr = entry->xen;
			ist_okay = entry->ist_okay;
			break;
		}
	}

	if (nr == ARRAY_SIZE(trap_array) &&
	    *addr >= (void *)early_idt_handler_array[0] &&
	    *addr < (void *)early_idt_handler_array[NUM_EXCEPTION_VECTORS]) {
		nr = (*addr - (void *)early_idt_handler_array[0]) /
		     EARLY_IDT_HANDLER_SIZE;
		*addr = (void *)xen_early_idt_handler_array[nr];
	}

	if (WARN_ON(ist != 0 && !ist_okay))
		return false;

	return true;
}