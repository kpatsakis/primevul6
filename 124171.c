DDeviceDiskInfo::DDeviceDiskInfo(const QString &filePath)
{
    const QJsonArray &block_devices = Helper::getBlockDevices(filePath);

    if (!block_devices.isEmpty()) {
        const QJsonObject &obj = block_devices.first().toObject();

        d = new DDeviceDiskInfoPrivate(this);
        d_func()->init(obj);

        if (d->type == Part) {
            const QJsonArray &parent = Helper::getBlockDevices(obj.value("pkname").toString());

            if (!parent.isEmpty()) {
                const QJsonObject &parent_obj = parent.first().toObject();

                d->transport = parent_obj.value("tran").toString();
                d->model = parent_obj.value("model").toString();
                d->serial = parent_obj.value("serial").toString();
            }

            if (!d->children.isEmpty())
                d->children.first().d->transport = d->transport;
        }
    }
}