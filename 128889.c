libxlDomainObjFreeJob(libxlDomainObjPrivate *priv)
{
    ignore_value(virCondDestroy(&priv->job.cond));
    VIR_FREE(priv->job.current);
}