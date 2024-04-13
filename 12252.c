testPathRelative(const void *args)
{
    const struct testPathRelativeBacking *data = args;
    g_autofree char *actual = NULL;

    if (virStorageFileGetRelativeBackingPath(data->top,
                                             data->base,
                                             &actual) < 0) {
        fprintf(stderr, "relative backing path resolution failed\n");
        return -1;
    }

    if (STRNEQ_NULLABLE(data->expect, actual)) {
        fprintf(stderr, "relative path resolution from '%s' to '%s': "
                "expected '%s', got '%s'\n",
                data->top->path, data->base->path,
                NULLSTR(data->expect), NULLSTR(actual));
        return -1;
    }

    return 0;
}