libxlDomainObjResetJob(libxlDomainObjPrivate *priv)
{
    struct libxlDomainJobObj *job = &priv->job;

    job->active = LIBXL_JOB_NONE;
    job->owner = 0;
}