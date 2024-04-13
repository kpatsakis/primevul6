AddressSpace *cpu_get_address_space(CPUState *cpu, int asidx)
{
    /* Return the AddressSpace corresponding to the specified index */
    return cpu->cpu_ases[asidx].as;
}