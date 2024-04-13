libxlDomainJobUpdateTime(struct libxlDomainJobObj *job)
{
    virDomainJobInfoPtr jobInfo = job->current;
    unsigned long long now;

    if (!job->started)
        return 0;

    if (virTimeMillisNow(&now) < 0)
        return -1;

    if (now < job->started) {
        job->started = 0;
        return 0;
    }

    jobInfo->timeElapsed = now - job->started;
    return 0;
}