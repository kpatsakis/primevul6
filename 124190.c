static QString getPTName(const QString &device)
{
    Helper::processExec(QStringLiteral("/sbin/blkid -p -s PTTYPE -d -i %1").arg(device));

    const QByteArray &data = Helper::lastProcessStandardOutput();

    if (data.isEmpty())
        return QString();

    const QByteArrayList &list = data.split('=');

    if (list.count() != 3)
        return QString();

    return list.last().simplified();
}