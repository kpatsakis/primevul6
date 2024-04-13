libxlDomainFreeMem(libxl_ctx *ctx, libxl_domain_config *d_config)
{
    uint64_t needed_mem;
    uint64_t free_mem;
    uint64_t target_mem;
    int tries = 3;
    int wait_secs = 10;

    if (libxlDomainNeedMemoryWrapper(ctx, d_config, &needed_mem) < 0)
        goto error;

    do {
        if (libxlGetFreeMemoryWrapper(ctx, &free_mem) < 0)
            goto error;

        if (free_mem >= needed_mem)
            return 0;

        target_mem = free_mem - needed_mem;
        if (libxlSetMemoryTargetWrapper(ctx, 0, target_mem,
                                        /* relative */ 1, 0) < 0)
            goto error;

        if (libxl_wait_for_memory_target(ctx, 0, wait_secs) < 0)
            goto error;

        tries--;
    } while (tries > 0);

 error:
    virReportError(VIR_ERR_OPERATION_FAILED, "%s",
                   _("Failed to balloon domain0 memory"));
    return -1;
}