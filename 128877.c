libxlDomainDefCheckABIStability(libxlDriverPrivate *driver,
                                virDomainDef *src,
                                virDomainDef *dst)
{
    virDomainDef *migratableDefSrc = NULL;
    virDomainDef *migratableDefDst = NULL;
    bool ret = false;

    if (!(migratableDefSrc = virDomainDefCopy(src, driver->xmlopt, NULL, true)) ||
        !(migratableDefDst = virDomainDefCopy(dst, driver->xmlopt, NULL, true)))
        goto cleanup;

    ret = virDomainDefCheckABIStability(migratableDefSrc,
                                        migratableDefDst,
                                        driver->xmlopt);

 cleanup:
    virDomainDefFree(migratableDefSrc);
    virDomainDefFree(migratableDefDst);
    return ret;
}