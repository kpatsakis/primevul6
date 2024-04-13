int Helper::processExec(const QString &command, int timeout)
{
    QProcess process;

    return processExec(&process, command, timeout);
}