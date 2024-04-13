DDeviceDiskInfoPrivate::~DDeviceDiskInfoPrivate()
{
    closeDataStream();

    if (process)
        process->deleteLater();
}