QString Helper::parseSerialUrl(const QString &urlString, QString *errorString)
{
    if (urlString.isEmpty())
        return QString();

    const QUrl url(urlString);
    const QString serial_number = urlString.split("//").at(1).split(":").first();
    const int part_index = url.port();
    const QString &path = url.path();
    const QString &device = Helper::findDiskBySerialIndexNumber(serial_number, part_index);
    const QString &device_url = part_index > 0 ? QString("serial://%1:%2").arg(serial_number).arg(part_index) : "serial://" + serial_number;

    if (device.isEmpty()) {
        if (errorString) {
            if (part_index > 0)
                *errorString = QObject::tr("Partition \"%1\" not found").arg(device_url);
            else
                *errorString = QObject::tr("Disk \"%1\" not found").arg(device_url);
        }

        return device;
    }

    if (path.isEmpty())
        return device;

    const QString &mp = Helper::mountPoint(device);

    QDir mount_point(mp);

    if (mp.isEmpty()) {
        QString mount_name;

        if (part_index >= 0)
            mount_name = QString("%1-%2").arg(serial_number).arg(part_index);
        else
            mount_name = serial_number;

        const QString &_mount_point = Helper::temporaryMountDevice(device, mount_name);

        if (_mount_point.isEmpty()) {
            if (errorString)
                *errorString = QObject::tr("Failed to mount partition \"%1\"").arg(device_url);

            return QString();
        }

        mount_point.setPath(_mount_point);
    }

    if (mount_point.absolutePath() == "/")
        return path;

    return mount_point.absolutePath() + path;
}