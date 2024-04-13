void DDeviceDiskInfoPrivate::refresh()
{
    children.clear();

    const QJsonArray &block_devices = Helper::getBlockDevices(name);

    if (!block_devices.isEmpty())
        init(block_devices.first().toObject());
}