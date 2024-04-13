libxlDomainDestroyInternal(libxlDriverPrivate *driver,
                           virDomainObj *vm)
{
    g_autoptr(libxlDriverConfig) cfg = libxlDriverConfigGet(driver);
    libxlDomainObjPrivate *priv = vm->privateData;
    int ret = -1;

    if (priv->deathW) {
        libxl_evdisable_domain_death(cfg->ctx, priv->deathW);
        priv->deathW = NULL;
    }

    /* Unlock virDomainObj during destroy, which can take considerable
     * time on large memory domains.
     */
    virObjectUnlock(vm);
    ret = libxl_domain_destroy(cfg->ctx, vm->def->id, NULL);
    virObjectLock(vm);

    return ret;
}