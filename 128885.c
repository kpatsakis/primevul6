libxlDomainObjInitJob(libxlDomainObjPrivate *priv)
{
    memset(&priv->job, 0, sizeof(priv->job));

    if (virCondInit(&priv->job.cond) < 0)
        return -1;

    priv->job.current = g_new0(virDomainJobInfo, 1);

    return 0;
}