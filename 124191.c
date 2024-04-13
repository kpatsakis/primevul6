QByteArray Helper::getPartitionTable(const QString &devicePath)
{
    processExec(QStringLiteral("/sbin/sfdisk -d %1").arg(devicePath));

    return lastProcessStandardOutput();
}