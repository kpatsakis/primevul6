rfbDefaultClientLog(const char *format, ...)
{
    va_list args;
    char buf[256];
    time_t log_clock;

    if(!rfbEnableClientLogging)
      return;

    va_start(args, format);

    time(&log_clock);
    strftime(buf, 255, "%d/%m/%Y %X ", localtime(&log_clock));
    fprintf(stderr, "%s", buf);

    vfprintf(stderr, format, args);
    fflush(stderr);

    va_end(args);
}