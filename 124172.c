int Helper::clonePartition(const DPartInfo &part, const QString &to, bool override)
{
    QString executor = getPartcloneExecuter(part);
    QString command;

    if (executor.isEmpty() || executor == "partclone.imager") {
        if (part.guidType() == DPartInfo::InvalidGUID)
            return -1;

        command = QStringLiteral("dd if=%1 of=%2 status=none conv=fsync").arg(part.filePath()).arg(to);
    } else if (isBlockSpecialFile(to)) {
        command = QStringLiteral("/usr/sbin/%1 -b -c -s %2 -%3 %4").arg(executor).arg(part.filePath()).arg(override ? "O" : "o").arg(to);
    } else {
        command = QStringLiteral("/usr/sbin/%1 -c -s %2 -%3 %4").arg(executor).arg(part.filePath()).arg(override ? "O" : "o").arg(to);
    }

    int code = processExec(command);

    if (code != 0)
        qDebug() << command << QString::fromUtf8(lastProcessStandardOutput());

    return code;
}