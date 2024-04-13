qint64 DDeviceDiskInfoPrivate::write(const char *data, qint64 maxSize)
{
    if (!process)
        return -1;

    if (process->state() != QProcess::Running)
        return -1;

    qint64 size = process->write(data, maxSize);

    QElapsedTimer timer;

    timer.start();

    int timeout = 5000;

    while (process->state() == QProcess::Running && process->bytesToWrite() > 0) {
        process->waitForBytesWritten();

        if (timer.elapsed() > timeout) {
            timeout += 5000;

            dCWarning("Wait for bytes written timeout, elapsed: %lld, bytes to write: %lld", timer.elapsed(), process->bytesToWrite());
        }
    }

    return size;
}