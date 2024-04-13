QString Helper::parentDevice(const QString &device)
{
    const QJsonArray &blocks = getBlockDevices(device);

    if (blocks.isEmpty())
        return device;

    const QString &parent = blocks.first().toObject().value("pkname").toString();

    if (parent.isEmpty())
        return device;

    return parent;
}