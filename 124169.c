bool Helper::existLiveSystem()
{
    return QFile::exists("/recovery");
}