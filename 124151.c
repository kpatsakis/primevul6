int Helper::restorePartition(const QString &from, const DPartInfo &to)
{
    QString command;

    if (isPartcloneFile(from)) {
        command = QStringLiteral("/usr/sbin/partclone.restore -s %1 -o %2").arg(from).arg(to.filePath());
    } else {
        command = QStringLiteral("dd if=%1 of=%2 status=none conv=fsync").arg(from).arg(to.filePath());
    }

    int code = processExec(command);

    if (code != 0)
        qDebug() << command << QString::fromUtf8(lastProcessStandardOutput());

    return code;
}