bool Helper::resetPartUUID(const DPartInfo &part, QByteArray uuid)
{
    QString command;

    if (uuid.isEmpty()) {
        uuid = QUuid::createUuid().toByteArray().mid(1, 36);
    }

    switch (part.fileSystemType()) {
    case DPartInfo::EXT2:
    case DPartInfo::EXT3:
    case DPartInfo::EXT4:
        command = QString("tune2fs -U %1 %2").arg(QString::fromLatin1(uuid)).arg(part.filePath());
        break;
    case DPartInfo::JFS:
        command = QString("jfs_tune -U %1 %2").arg(QString::fromLatin1(uuid)).arg(part.filePath());
        break;
    case DPartInfo::NTFS:
        command = QString("ntfslabel --new-half-serial %1").arg(part.filePath());
        break;
    case DPartInfo::XFS:
        command = QString("xfs_admin -U %1 %2").arg(QString::fromLatin1(uuid)).arg(part.filePath());
        break;
    default:
        dCDebug("Not support the file system type: %s", qPrintable(part.fileSystemTypeName()));

        return false;
    }

    if (!umountDevice(part.filePath())) {
        dCDebug("Failed to umount the partition: %s", qPrintable(part.filePath()));

        return false;
    }

    // check the partition
    processExec("fsck -f -y " + part.filePath());

    bool ok = processExec(command) == 0;

    if (!ok) {
        dCError("Failed reset part uuid");
        dCDebug(qPrintable(lastProcessStandardOutput()));
        dCError(qPrintable(lastProcessStandardError()));
    }

    return ok;
}