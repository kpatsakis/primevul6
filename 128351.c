void cpu_address_space_init(CPUState *cpu, AddressSpace *as, int asidx)
{
    CPUAddressSpace *newas;

    /* Target code should have set num_ases before calling us */
    assert(asidx < cpu->num_ases);

    if (asidx == 0) {
        /* address space 0 gets the convenience alias */
        cpu->as = as;
    }

    /* KVM cannot currently support multiple address spaces. */
    assert(asidx == 0 || !kvm_enabled());

    if (!cpu->cpu_ases) {
        cpu->cpu_ases = g_new0(CPUAddressSpace, cpu->num_ases);
    }

    newas = &cpu->cpu_ases[asidx];
    newas->cpu = cpu;
    newas->as = as;
    if (tcg_enabled()) {
        newas->tcg_as_listener.commit = tcg_commit;
        memory_listener_register(&newas->tcg_as_listener, as);
    }
}