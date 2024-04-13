testStorageChain(const void *args)
{
    const struct testChainData *data = args;
    virStorageSourcePtr elt;
    size_t i = 0;
    g_autoptr(virStorageSource) meta = NULL;
    g_autofree char *broken = NULL;

    meta = testStorageFileGetMetadata(data->start, data->format, -1, -1);
    if (!meta) {
        if (data->flags & EXP_FAIL) {
            virResetLastError();
            return 0;
        }
        return -1;
    } else if (data->flags & EXP_FAIL) {
        fprintf(stderr, "call should have failed\n");
        return -1;
    }

    if (virGetLastErrorCode()) {
        fprintf(stderr, "call should not have reported error\n");
        return -1;
    }

    if (virStorageFileChainGetBroken(meta, &broken) || broken) {
        fprintf(stderr, "chain should not be identified as broken\n");
        return -1;
    }

    elt = meta;
    while (virStorageSourceIsBacking(elt)) {
        g_autofree char *expect = NULL;
        g_autofree char *actual = NULL;

        if (i == data->nfiles) {
            fprintf(stderr, "probed chain was too long\n");
            return -1;
        }

        expect = g_strdup_printf(testStorageChainFormat, i,
                                 NULLSTR(data->files[i]->path),
                                 NULLSTR(data->files[i]->expBackingStoreRaw),
                                 data->files[i]->expCapacity,
                                 data->files[i]->expEncrypted,
                                 NULLSTR(data->files[i]->pathRel),
                                 data->files[i]->type,
                                 data->files[i]->format,
                                 virStorageNetProtocolTypeToString(data->files[i]->protocol),
                                 NULLSTR(data->files[i]->hostname));
        actual = g_strdup_printf(testStorageChainFormat, i,
                                 NULLSTR(elt->path),
                                 NULLSTR(elt->backingStoreRaw),
                                 elt->capacity,
                                 !!elt->encryption,
                                 NULLSTR(elt->relPath),
                                 elt->type,
                                 elt->format,
                                 virStorageNetProtocolTypeToString(elt->protocol),
                                 NULLSTR(elt->nhosts ? elt->hosts[0].name : NULL));
        if (STRNEQ(expect, actual)) {
            virTestDifference(stderr, expect, actual);
            return -1;
        }
        elt = elt->backingStore;
        i++;
    }
    if (i != data->nfiles) {
        fprintf(stderr, "probed chain was too short\n");
        return -1;
    }

    return 0;
}