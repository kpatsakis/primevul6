QString Helper::getPartcloneExecuter(const DPartInfo &info)
{
    QString executor;

    switch (info.fileSystemType()) {
    case DPartInfo::Invalid:
        break;
    case DPartInfo::Btrfs:
        executor = "btrfs";
        break;
    case DPartInfo::EXT2:
    case DPartInfo::EXT3:
    case DPartInfo::EXT4:
        executor = "extfs";
        break;
    case DPartInfo::F2FS:
        executor = "f2fs";
        break;
    case DPartInfo::FAT12:
    case DPartInfo::FAT16:
    case DPartInfo::FAT32:
        executor = "fat";
        break;
    case DPartInfo::HFS_Plus:
        executor = "hfsplus";
        break;
    case DPartInfo::Minix:
        executor = "minix";
        break;
    case DPartInfo::Nilfs2:
        executor = "nilfs2";
        break;
    case DPartInfo::NTFS:
        executor = "ntfs -I";
        break;
    case DPartInfo::Reiser4:
        executor = "reiser4";
        break;
    case DPartInfo::VFAT:
        executor = "vfat";
        break;
    case DPartInfo::XFS:
        executor = "xfs";
        break;
    default:
        if (!QStandardPaths::findExecutable("partclone." + info.fileSystemTypeName().toLower()).isEmpty())
            executor = info.fileSystemTypeName().toLower();
        break;
    }

    if (executor.isEmpty())
        return "partclone.imager";

    return "partclone." + executor;
}