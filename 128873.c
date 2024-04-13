libxlDomainStartPerform(libxlDriverPrivate *driver,
                        virDomainObj *vm,
                        bool start_paused,
                        int restore_fd,
                        uint32_t restore_ver)
{
    libxl_domain_config d_config;
    int ret = -1;
    int libxlret = -1;
    uint32_t domid = 0;
    g_autofree char *dom_xml = NULL;
    libxlDomainObjPrivate *priv = vm->privateData;
    g_autoptr(libxlDriverConfig) cfg = libxlDriverConfigGet(driver);
    libxl_asyncprogress_how aop_console_how;
    libxl_domain_restore_params params;
    g_autofree char *config_json = NULL;

    libxl_domain_config_init(&d_config);

    if (libxlBuildDomainConfig(driver->reservedGraphicsPorts, vm->def,
                               cfg, &d_config) < 0)
        goto cleanup;

    if (cfg->autoballoon && libxlDomainFreeMem(cfg->ctx, &d_config) < 0)
        goto cleanup;

    /* now that we know it is about to start call the hook if present */
    if (libxlDomainHookRun(driver, vm->def, 0,
                           VIR_HOOK_LIBXL_OP_START,
                           VIR_HOOK_SUBOP_BEGIN, NULL) < 0)
        goto cleanup;

    if (priv->hookRun) {
        char uuidstr[VIR_UUID_STRING_BUFLEN];
        virUUIDFormat(vm->def->uuid, uuidstr);

        VIR_WARN("Domain id='%d' name='%s' uuid='%s' is tainted: hook",
                 vm->def->id,
                 vm->def->name,
                 uuidstr);
    }

    /* Unlock virDomainObj while creating the domain */
    virObjectUnlock(vm);

    aop_console_how.for_callback = vm;
    aop_console_how.callback = libxlConsoleCallback;
    if (restore_fd < 0) {
        libxlret = libxl_domain_create_new(cfg->ctx, &d_config,
                                      &domid, NULL, &aop_console_how);
    } else {
        libxl_domain_restore_params_init(&params);
        params.stream_version = restore_ver;
        libxlret = libxlDomainCreateRestoreWrapper(cfg->ctx, &d_config, &domid,
                                              restore_fd, &params,
                                          &aop_console_how);
        libxl_domain_restore_params_dispose(&params);
    }
    virObjectLock(vm);

    if (libxlret) {
        if (restore_fd < 0)
            virReportError(VIR_ERR_INTERNAL_ERROR,
                           _("libxenlight failed to create new domain '%s'"),
                           d_config.c_info.name);
        else
            virReportError(VIR_ERR_INTERNAL_ERROR,
                           _("libxenlight failed to restore domain '%s'"),
                           d_config.c_info.name);
        goto cleanup;
    }

    /*
     * The domain has been successfully created with libxl, so it should
     * be cleaned up if there are any subsequent failures.
     */
    vm->def->id = domid;
    config_json = libxl_domain_config_to_json(cfg->ctx, &d_config);

    libxlLoggerOpenFile(cfg->logger, domid, vm->def->name, config_json);

    if (virDomainLockProcessResume(driver->lockManager,
                                  "xen:///system",
                                  vm,
                                  priv->lockState) < 0)
        goto destroy_dom;
    VIR_FREE(priv->lockState);
    priv->lockProcessRunning = true;

    /* Always enable domain death events */
    if (libxl_evenable_domain_death(cfg->ctx, vm->def->id, 0, &priv->deathW))
        goto destroy_dom;

    libxlDomainCreateIfaceNames(vm->def, &d_config);
    libxlDomainUpdateDiskParams(vm->def, cfg->ctx);

    if (vm->def->nchannels > 0)
        libxlDomainCreateChannelPTY(vm->def, cfg->ctx);

    if ((dom_xml = virDomainDefFormat(vm->def, driver->xmlopt, 0)) == NULL)
        goto destroy_dom;

    if (libxl_userdata_store(cfg->ctx, domid, "libvirt-xml",
                             (uint8_t *)dom_xml, strlen(dom_xml) + 1)) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("libxenlight failed to store userdata"));
        goto destroy_dom;
    }

    if (!start_paused) {
        libxlDomainUnpauseWrapper(cfg->ctx, domid);
        virDomainObjSetState(vm, VIR_DOMAIN_RUNNING, VIR_DOMAIN_RUNNING_BOOTED);
    } else {
        virDomainObjSetState(vm, VIR_DOMAIN_PAUSED, VIR_DOMAIN_PAUSED_USER);
    }

    if (virDomainObjSave(vm, driver->xmlopt, cfg->stateDir) < 0)
        goto destroy_dom;

    /* finally we can call the 'started' hook script if any */
    if (libxlDomainHookRun(driver, vm->def, 0,
                           VIR_HOOK_LIBXL_OP_STARTED,
                           VIR_HOOK_SUBOP_BEGIN, NULL) < 0)
        goto destroy_dom;

    ret = 0;
    goto cleanup;

 destroy_dom:
    libxlDomainDestroyInternal(driver, vm);
    vm->def->id = -1;
    virDomainObjSetState(vm, VIR_DOMAIN_SHUTOFF, VIR_DOMAIN_SHUTOFF_FAILED);

 cleanup:
    libxl_domain_config_dispose(&d_config);
    return ret;
}