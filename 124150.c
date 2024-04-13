QString DDeviceDiskInfoPrivate::errorString() const
{
    if (error.isEmpty()) {
        if (process) {
            if (process->error() == QProcess::UnknownError)
                return QString();

            return QString("%1 %2: %3").arg(process->program()).arg(process->arguments().join(' ')).arg(process->errorString());
        }

        if (!buffer.QIODevice::d_func()->errorString.isEmpty())
            return buffer.errorString();
    }

    return error;
}