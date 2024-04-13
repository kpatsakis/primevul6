QByteArray Helper::callLsblk(const QString &extraArg)
{
    processExec(COMMAND_LSBLK.arg(extraArg));

    return lastProcessStandardOutput();
}