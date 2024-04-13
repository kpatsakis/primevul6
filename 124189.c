bool Helper::isBlockSpecialFile(const QString &fileName)
{
    if (fileName.startsWith("/dev/"))
        return true;

    processExec(QStringLiteral("env LANG=C stat -c %F %1").arg(fileName));

    return lastProcessStandardOutput() == "block special file\n";
}