testPathCanonicalize(const void *args)
{
    const struct testPathCanonicalizeData *data = args;
    g_autofree char *canon = NULL;

    canon = virStorageFileCanonicalizePath(data->path,
                                           testPathCanonicalizeReadlink,
                                           NULL);

    if (STRNEQ_NULLABLE(data->expect, canon)) {
        fprintf(stderr,
                "path canonicalization of '%s' failed: expected '%s' got '%s'\n",
                data->path, NULLSTR(data->expect), NULLSTR(canon));

        return -1;
    }

    return 0;
}