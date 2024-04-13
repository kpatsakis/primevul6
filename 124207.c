QJsonArray Helper::getBlockDevices(const QString &commandExtraArg)
{
    const QByteArray &array = Helper::callLsblk(commandExtraArg);

    QJsonParseError error;

    const QJsonDocument &jd = QJsonDocument::fromJson(QString::fromUtf8(array).toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        dCError(error.errorString());
    }

    return jd.object().value("blockdevices").toArray();
}