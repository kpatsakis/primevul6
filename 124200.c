bool Helper::mountDevice(const QString &device, const QString &path, bool readonly)
{
    if (readonly)
        return processExec(QString("mount -r %1 %2").arg(device, path)) == 0;

    return processExec(QString("mount %1 %2").arg(device, path)) == 0;
}