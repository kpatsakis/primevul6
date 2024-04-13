bool Helper::deviceHaveKinship(const QString &device1, const QString &device2)
{
    return device1 == device2 || parentDevice(device1) == parentDevice(device2);
}