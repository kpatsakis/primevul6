bool Helper::tryUmountDevice(const QString &device)
{
    const QJsonArray &array = getBlockDevices("-l " + device);

    for (const QJsonValue &device : array) {
        const QJsonObject &obj = device.toObject();

        if (!obj.value("mountpoint").isNull()) {
            if (processExec(QString("umount -d %1 --fake").arg(obj.value("name").toString())) != 0)
                return false;
        }
    }

    return true;
}