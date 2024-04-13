qint64 DDeviceDiskInfoPrivate::totalReadableDataSize() const
{
    qint64 size = 0;

    if (hasScope(DDiskInfo::PartitionTable, DDiskInfo::Read)) {
        if (hasScope(DDiskInfo::Headgear, DDiskInfo::Read)) {
            size += 1048576;
        } else if (!children.isEmpty()) {
            size += children.first().sizeStart();
        }

        if (ptType == DDiskInfo::MBR) {
            size += 512;
        } else if (ptType == DDiskInfo::GPT) {
            size += 17408;
            size += 16896;
        }
    }

    for (const DPartInfo &part : children) {
        if (!part.isExtended())
            size += part.usedSize();
    }

    return size;
}