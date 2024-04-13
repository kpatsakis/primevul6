qint64 DDeviceDiskInfoPrivate::read(char *data, qint64 maxSize)
{
    if (!process) {
        return buffer.read(data, maxSize);
    }

    process->waitForReadyRead(-1);

    if (process->bytesAvailable() > Global::bufferSize) {
        dCWarning("The \"%s %s\" process bytes available: %s", qPrintable(process->program()), qPrintable(process->arguments().join(" ")), qPrintable(Helper::sizeDisplay(process->bytesAvailable())));
    }

    return process->read(data, maxSize);
}