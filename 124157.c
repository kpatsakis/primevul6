bool Helper::isMounted(const QString &device)
{
    const QJsonArray &array = getBlockDevices("-l " + device);

    for (const QJsonValue &part : array) {
        const QJsonObject &obj = part.toObject();

        if (!obj.value("mountpoint").isNull())
            return true;
    }

    return false;
}