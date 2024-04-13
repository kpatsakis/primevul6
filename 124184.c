QString Helper::sizeDisplay(qint64 size)
{
    constexpr qreal kb = 1024;
    constexpr qreal mb = kb * 1024;
    constexpr qreal gb = mb * 1024;
    constexpr qreal tb = gb * 1024;

    if (size > tb)
        return QString::asprintf("%.2f TB", size / tb);

    if (size > gb)
        return QString::asprintf("%.2f GB", size / gb);

    if (size > mb)
        return QString::asprintf("%.2f MB", size / mb);

    if (size > kb)
        return QString::asprintf("%.2f KB", size / kb);

    return QString("%1 B").arg(size);
}