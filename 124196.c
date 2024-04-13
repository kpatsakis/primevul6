QString Helper::getDeviceForFile(const QString &file, QString *rootPath)
{
    if (file.isEmpty())
        return QString();

    if (Helper::isBlockSpecialFile(file))
        return file;

    QFileInfo info(file);

    while (!info.exists() && info.absoluteFilePath() != "/")
        info.setFile(info.absolutePath());

    QStorageInfo storage_info(info.absoluteFilePath());

    if (rootPath)
        *rootPath = storage_info.rootPath();

    return QString::fromUtf8(storage_info.device());
}