bool DDeviceDiskInfoPrivate::hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode, int index) const
{
    if (mode == DDiskInfo::Read) {
        if (scope == DDiskInfo::Headgear) {
            return havePartitionTable && (children.isEmpty() || children.first().sizeStart() >= 1048576);
        } else if (scope == DDiskInfo::JsonInfo) {
            return true;
        }

        if (scope == DDiskInfo::PartitionTable)
            return havePartitionTable;
    } else if (readonly || scope == DDiskInfo::JsonInfo) {
        return false;
    }

    if (scope == DDiskInfo::Partition) {
        if (index == 0 && mode == DDiskInfo::Write)
            return true;

        const DPartInfo &info = q->getPartByNumber(index);

        if (!info) {
            dCDebug("Can not find parition by number(device: \"%s\"): %d", qPrintable(q->filePath()), index);

            return false;
        }

        if (info.isExtended() || (mode == DDiskInfo::Read
                                  && info.type() == DPartInfo::Unknow
                                  && info.fileSystemType() == DPartInfo::Invalid
                                  && info.guidType() == DPartInfo::InvalidGUID)) {
            dCDebug("Skip the \"%s\" partition, type: %s", qPrintable(info.filePath()), qPrintable(info.typeDescription(info.type())));

            return false;
        }

        return mode != DDiskInfo::Write || !info.isReadonly();
    }

    return (scope == DDiskInfo::Headgear || scope == DDiskInfo::PartitionTable) ? type == DDiskInfo::Disk : true;
}