libxlDomainObjPrivateFree(void *data)
{
    libxlDomainObjPrivate *priv = data;

    g_free(priv->lockState);
    libxlDomainObjFreeJob(priv);
    virChrdevFree(priv->devs);
    g_free(priv);
}