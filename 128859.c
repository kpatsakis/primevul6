libxlDomainDefPostParse(virDomainDef *def,
                        unsigned int parseFlags G_GNUC_UNUSED,
                        void *opaque G_GNUC_UNUSED,
                        void *parseOpaque G_GNUC_UNUSED)
{
    /* Xen PV domains always have a PV console, so add one to the domain config
     * via post-parse callback if not explicitly specified in the XML. */
    if (def->os.type != VIR_DOMAIN_OSTYPE_HVM && def->nconsoles == 0) {
        virDomainChrDef *chrdef;

        if (!(chrdef = virDomainChrDefNew(NULL)))
            return -1;

        chrdef->source->type = VIR_DOMAIN_CHR_TYPE_PTY;
        chrdef->deviceType = VIR_DOMAIN_CHR_DEVICE_TYPE_CONSOLE;
        chrdef->target.port = 0;
        chrdef->targetType = VIR_DOMAIN_CHR_CONSOLE_TARGET_TYPE_XEN;

        def->consoles = g_new0(virDomainChrDef *, 1);
        def->nconsoles = 1;
        def->consoles[0] = chrdef;
    }

    /* add implicit input devices */
    if (xenDomainDefAddImplicitInputDevice(def) < 0)
        return -1;

    /* For x86_64 HVM */
    if (def->os.type == VIR_DOMAIN_OSTYPE_HVM &&
        def->os.arch == VIR_ARCH_X86_64) {
        /* always enable pae */
        def->features[VIR_DOMAIN_FEATURE_PAE] = VIR_TRISTATE_SWITCH_ON;

        /* if vnuma is effective enable acpi */
        if (virDomainNumaGetNodeCount(def->numa) > 0)
            def->features[VIR_DOMAIN_FEATURE_ACPI] = VIR_TRISTATE_SWITCH_ON;
    }

    /* add implicit balloon device */
    if (def->memballoon == NULL) {
        virDomainMemballoonDef *memballoon;
        memballoon = g_new0(virDomainMemballoonDef,
                            1);

        memballoon->model = VIR_DOMAIN_MEMBALLOON_MODEL_XEN;
        def->memballoon = memballoon;
    }

    /* add implicit xenbus device */
    if (virDomainControllerFindByType(def, VIR_DOMAIN_CONTROLLER_TYPE_XENBUS) == -1)
        if (virDomainDefAddController(def, VIR_DOMAIN_CONTROLLER_TYPE_XENBUS, -1, -1) == NULL)
            return -1;

    return 0;
}