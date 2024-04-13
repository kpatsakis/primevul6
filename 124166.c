bool Helper::saveToFile(const QString &fileName, const QByteArray &data, bool override)
{
    if (!override && QFile::exists(fileName))
        return false;

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        dCError(file.errorString());

        return false;
    }

    qint64 size = file.write(data);
    file.flush();
    file.close();

    return size == data.size();
}