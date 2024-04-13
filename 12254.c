testStorageLookup(const void *args)
{
    const struct testLookupData *data = args;
    int ret = 0;
    virStorageSourcePtr result;
    virStorageSourcePtr actualParent;
    unsigned int idx;

    if (virStorageFileParseChainIndex(data->target, data->name, &idx) < 0 &&
        data->expIndex) {
        fprintf(stderr, "call should not have failed\n");
        ret = -1;
    }
    if (idx != data->expIndex) {
        fprintf(stderr, "index: expected %u, got %u\n", data->expIndex, idx);
        ret = -1;
    }

     /* Test twice to ensure optional parameter doesn't cause NULL deref. */
    result = virStorageFileChainLookup(data->chain, data->from,
                                       idx ? NULL : data->name,
                                       idx, NULL);

    if (!data->expResult) {
        if (virGetLastErrorCode() == VIR_ERR_OK) {
            fprintf(stderr, "call should have failed\n");
            ret = -1;
        }
        virResetLastError();
    } else {
        if (virGetLastErrorCode()) {
            fprintf(stderr, "call should not have warned\n");
            ret = -1;
        }
    }

    if (!result) {
        if (data->expResult) {
            fprintf(stderr, "result 1: expected %s, got NULL\n",
                    data->expResult);
            ret = -1;
        }
    } else if (STRNEQ_NULLABLE(data->expResult, result->path)) {
        fprintf(stderr, "result 1: expected %s, got %s\n",
                NULLSTR(data->expResult), NULLSTR(result->path));
        ret = -1;
    }

    result = virStorageFileChainLookup(data->chain, data->from,
                                       data->name, idx, &actualParent);
    if (!data->expResult)
        virResetLastError();

    if (!result) {
        if (data->expResult) {
            fprintf(stderr, "result 2: expected %s, got NULL\n",
                    data->expResult);
            ret = -1;
        }
    } else if (STRNEQ_NULLABLE(data->expResult, result->path)) {
        fprintf(stderr, "result 2: expected %s, got %s\n",
                NULLSTR(data->expResult), NULLSTR(result->path));
        ret = -1;
    }
    if (data->expMeta != result) {
        fprintf(stderr, "meta: expected %p, got %p\n",
                data->expMeta, result);
        ret = -1;
    }
    if (data->expParent != actualParent) {
        fprintf(stderr, "parent: expected %s, got %s\n",
                NULLSTR(data->expParent ? data->expParent->path : NULL),
                NULLSTR(actualParent ? actualParent->path : NULL));
        ret = -1;
    }

    return ret;
}