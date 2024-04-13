failed(const char *msg)
{
    int code = errno;

    (void) fprintf(stderr, "%s: %s: %s\n", _nc_progname, msg, strerror(code));
    restore_tty_settings();
    (void) fprintf(my_file, "\n");
    fflush(my_file);
    ExitProgram(ErrSystem(code));
    /* NOTREACHED */
}