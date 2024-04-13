QString Helper::findDiskBySerialIndexNumber(const QString &serialNumber, int partIndexNumber)
{
    const QJsonArray &array = getBlockDevices();

    for (const QJsonValue &disk : array) {
        const QJsonObject &obj = disk.toObject();

        if (obj.value("serial").toString().compare(serialNumber, Qt::CaseInsensitive) != 0) {
            continue;
        }

        if (partIndexNumber <= 0)
            return obj.value("name").toString();

        const QJsonArray &children = obj.value("children").toArray();

        for (const QJsonValue &v : children) {
            const QJsonObject &obj = v.toObject();
            const QString &name = obj.value("name").toString();

            if (DDevicePartInfo(name).indexNumber() == partIndexNumber)
                return name;
        }
    }

    return QString();
}