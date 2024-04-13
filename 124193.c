void DDeviceDiskInfoPrivate::closeDataStream()
{
    closing = true;

    if (process) {
        if (process->state() != QProcess::NotRunning) {
            if (currentMode == DDiskInfo::Read) {
                process->closeReadChannel(QProcess::StandardOutput);
                process->terminate();
            } else {
                process->closeWriteChannel();
            }

            while (process->state() != QProcess::NotRunning) {
                QThread::currentThread()->sleep(1);

                if (!QFile::exists(QString("/proc/%2").arg(process->pid()))) {
                    process->waitForFinished(-1);

                    if (process->error() == QProcess::Timedout)
                        process->QIODevice::d_func()->errorString.clear();

                    break;
                }
            }
        }

        dCDebug("Process exit code: %d(%s %s)", process->exitCode(), qPrintable(process->program()), qPrintable(process->arguments().join(' ')));
    }

    if (currentMode == DDiskInfo::Write && currentScope == DDiskInfo::PartitionTable) {
        Helper::umountDevice(filePath());

        if (Helper::refreshSystemPartList(filePath())) {
            refresh();
        } else {
            dCWarning("Refresh the devcie %s failed", qPrintable(filePath()));
        }
    }

    if (currentScope == DDiskInfo::JsonInfo)
        buffer.close();

    closing = false;
}