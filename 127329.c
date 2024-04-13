static inline void write_idt_entry(gate_desc *dt, int entry, const gate_desc *g)
{
	PVOP_VCALL3(cpu.write_idt_entry, dt, entry, g);
}