bool Helper::restartToLiveSystem(const QStringList &arguments)
{
    if (!existLiveSystem()) {
        dCDebug("Not install live system");

        return false;
    }

    if (!QDir::current().mkpath("/recovery/.tmp")) {
        dCDebug("mkpath failed");

        return false;
    }

    QFile file("/recovery/.tmp/deepin-clone.arguments");

    if (!file.open(QIODevice::WriteOnly)) {
        dCDebug("Open file failed: \"%s\"", qPrintable(file.fileName()));

        return false;
    }

    file.write(arguments.join('\n').toUtf8());
    file.close();

    if (processExec("grub-reboot \"Deepin Recovery\"") != 0) {
        dCDebug("Exec grub-reboot \"Deepin Recovery\" failed");

        file.remove();

        return false;
    }

    if (processExec("reboot") != 0)
        file.remove();

    return true;
}