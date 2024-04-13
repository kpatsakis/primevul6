void cpu_exec_initfn(CPUState *cpu)
{
    cpu->as = NULL;
    cpu->num_ases = 0;

#ifndef CONFIG_USER_ONLY
    cpu->thread_id = qemu_get_thread_id();

    /* This is a softmmu CPU object, so create a property for it
     * so users can wire up its memory. (This can't go in qom/cpu.c
     * because that file is compiled only once for both user-mode
     * and system builds.) The default if no link is set up is to use
     * the system address space.
     */
    object_property_add_link(OBJECT(cpu), "memory", TYPE_MEMORY_REGION,
                             (Object **)&cpu->memory,
                             qdev_prop_allow_set_link_before_realize,
                             OBJ_PROP_LINK_UNREF_ON_RELEASE,
                             &error_abort);
    cpu->memory = system_memory;
    object_ref(OBJECT(cpu->memory));
#endif
}