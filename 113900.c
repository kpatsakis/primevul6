void error_callback(const char *msg, void *client_data)
{
    FILE *stream = (FILE*)client_data;
    fprintf(stream, "[ERROR] %s", msg);
}