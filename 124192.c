bool Helper::isDiskDevice(const QString &devicePath)
{
    const QJsonArray &blocks = getBlockDevices(devicePath);

    if (blocks.isEmpty())
        return false;

    if (!blocks.first().isObject())
        return false;

    return blocks.first().toObject().value("pkname").isNull();
}