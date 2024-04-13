qint64 DDeviceDiskInfoPrivate::maxReadableDataSize() const
{
    if (children.isEmpty()) {
        return totalReadableDataSize();
    }

    if (type == DDiskInfo::Disk)
        return children.last().sizeEnd() + 1;

    return children.first().totalSize();
}