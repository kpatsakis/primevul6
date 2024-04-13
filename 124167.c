QString Helper::toSerialUrl(const QString &file)
{
    if (file.isEmpty())
        return QString();

    if (Helper::isBlockSpecialFile(file)) {
        DDiskInfo info;

        if (Helper::isDiskDevice(file))
            info = DDiskInfo::getInfo(file);
        else
            info = DDiskInfo::getInfo(Helper::parentDevice(file));

        if (!info)
            return QString();

        if (info.serial().isEmpty())
            return QString();

        int index = DDevicePartInfo(file).indexNumber();

        if (index == 0)
            return "serial://" + info.serial();

        return QString("serial://%1:%2").arg(info.serial()).arg(index);
    }

    QString root_path;
    QString url = toSerialUrl(getDeviceForFile(file, &root_path));

    if (root_path == "/")
        return url + QFileInfo(file).absoluteFilePath();

    return url + QFileInfo(file).absoluteFilePath().mid(root_path.length());
}