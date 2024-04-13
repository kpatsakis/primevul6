failed(const char *msg)
{
    perror(msg);
    ExitProgram(EXIT_FAILURE);
}