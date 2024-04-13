testPathCanonicalizeReadlink(const char *path,
                             char **linkpath,
                             void *data G_GNUC_UNUSED)
{
    size_t i;

    *linkpath = NULL;

    for (i = 0; i < G_N_ELEMENTS(testPathCanonicalizeSymlinks); i++) {
        if (STREQ(path, testPathCanonicalizeSymlinks[i][0])) {
            *linkpath = g_strdup(testPathCanonicalizeSymlinks[i][1]);

            return 0;
        }
    }

    return 1;
}