QString Helper::mountPoint(const QString &device)
{
    const QJsonArray &array = getBlockDevices(device);

    if (array.isEmpty())
        return QString();

    return array.first().toObject().value("mountpoint").toString();
}