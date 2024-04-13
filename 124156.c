inline static bool isTUIMode(int argc, char *argv[])
{
#ifndef ENABLE_GUI
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    return true;
#endif

    if (qEnvironmentVariableIsEmpty("DISPLAY"))
        return true;

    const QByteArrayList in_tui_args = {
        "--tui", "-i", "--info", "--dim-info", "--to-serial-url",
        "--from-serial-url", "-f", "--fix-boot", "-v", "--version",
        "-h", "--help", "--re-checksum"
    };

    for (int i = 1; i < argc; ++i)
        if (in_tui_args.contains(argv[i]))
            return true;

    return false;
}