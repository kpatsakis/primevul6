bool Helper::refreshSystemPartList(const QString &device)
{
    int code = device.isEmpty() ? processExec("partprobe") : processExec(QString("partprobe %1").arg(device));

    if (code != 0)
        return false;

    QThread::sleep(1);

    return true;
}