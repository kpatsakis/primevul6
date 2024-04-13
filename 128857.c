libxlDomainObjPrivateAlloc(void *opaque G_GNUC_UNUSED)
{
    libxlDomainObjPrivate *priv;

    priv = g_new0(libxlDomainObjPrivate, 1);

    if (!(priv->devs = virChrdevAlloc())) {
        g_free(priv);
        return NULL;
    }

    if (libxlDomainObjInitJob(priv) < 0) {
        virChrdevFree(priv->devs);
        g_free(priv);
        return NULL;
    }

    return priv;
}