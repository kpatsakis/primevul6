bool DDeviceDiskInfoPrivate::atEnd() const
{
    if (!process) {
        return buffer.atEnd();
    }

    process->waitForReadyRead(-1);

    return process->atEnd();
}