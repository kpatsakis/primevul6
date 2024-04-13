QList<DDeviceDiskInfo> DDeviceDiskInfo::localeDiskList()
{
    const QJsonArray &block_devices = Helper::getBlockDevices();

    QList<DDeviceDiskInfo> list;

    for (const QJsonValue &value : block_devices) {
        const QJsonObject &obj = value.toObject();

        if (Global::disableLoopDevice && obj.value("type").toString() == "loop")
            continue;

        DDeviceDiskInfo info;

        info.d = new DDeviceDiskInfoPrivate(&info);
        info.d_func()->init(obj);
        list << info;
    }

    return list;
}