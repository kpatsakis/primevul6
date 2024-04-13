bool Helper::setPartitionTable(const QString &devicePath, const QString &ptFile)
{
    QProcess process;

    process.setStandardInputFile(ptFile);

    if (processExec(&process, QStringLiteral("/sbin/sfdisk %1").arg(devicePath)) != 0)
        return false;

    int code = processExec(QStringLiteral("/sbin/partprobe %1").arg(devicePath));
    processExec("sleep 1");

    return code == 0;
}