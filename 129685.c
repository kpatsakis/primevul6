static const char *nr2str(const char **n, size_t len, uint32_t nr)
{
    if (nr < len && n[nr] != NULL) {
        return n[nr];
    } else {
        return "unknown";
    }
}