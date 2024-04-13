failed(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    ExitProgram(EXIT_FAILURE);
}