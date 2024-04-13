QString Helper::secondsToString(qint64 seconds)
{
    int days = seconds / 86400;

    seconds = seconds % 86400;

    int hours = seconds / 3600;

    seconds = seconds % 3600;

    int minutes = seconds / 60;

    seconds = seconds % 60;

    if (days > 0)
        return QObject::tr("%1 d %2 h %3 m").arg(days).arg(hours).arg(minutes + 1);

    if (hours > 0)
        return QObject::tr("%1 h %2 m").arg(hours).arg(minutes + 1);

    if (minutes > 0)
        return QObject::tr("%1 m").arg(minutes + 1);

    return QObject::tr("%1 s").arg(seconds);
}