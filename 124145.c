int Helper::partitionIndexNumber(const QString &partDevice)
{
    const QJsonArray &array = getBlockDevices(partDevice);

    if (array.isEmpty())
        return -1;

    const QJsonArray &p_array = getBlockDevices(array.first().toObject().value("pkname").toString() + " -x NAME");

    if (p_array.isEmpty())
        return -1;

    const QJsonArray &part_list = p_array.first().toObject().value("children").toArray();

    for (int i = 0; i < part_list.count(); ++i) {
        const QJsonObject &obj = part_list.at(i).toObject();

        if (obj.value("name").toString() == partDevice || obj.value("kname").toString() == partDevice)
            return i;
    }

    return -1;
}