bool Helper::isDeepinSystem(const DPartInfo &part)
{
    QString mout_root = part.mountPoint();
    bool umount_device = false;

    if (mout_root.isEmpty()) {
        mout_root = temporaryMountDevice(part.name(), QFileInfo(part.name()).fileName(), true);

        if (mout_root.isEmpty())
            return false;

        umount_device = true;
    }

    bool is = QFile::exists(mout_root + "/etc/deepin-version");

    if (umount_device)
        umountDevice(part.name());

    return is;
}