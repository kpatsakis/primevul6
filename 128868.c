libxlDomainObjEndJob(libxlDriverPrivate *driver G_GNUC_UNUSED,
                     virDomainObj *obj)
{
    libxlDomainObjPrivate *priv = obj->privateData;
    enum libxlDomainJob job = priv->job.active;

    VIR_DEBUG("Stopping job: %s",
              libxlDomainJobTypeToString(job));

    libxlDomainObjResetJob(priv);
    virCondSignal(&priv->job.cond);
}