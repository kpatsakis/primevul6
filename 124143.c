bool Helper::isPartcloneFile(const QString &fileName)
{
    return processExec(QStringLiteral("partclone.info %1").arg(fileName)) == 0;
}