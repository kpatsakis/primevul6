testPathRelativePrepare(void)
{
    size_t i;

    for (i = 0; i < G_N_ELEMENTS(backingchain); i++) {
        backingchain[i].type = VIR_STORAGE_TYPE_FILE;
        if (i < G_N_ELEMENTS(backingchain) - 1)
            backingchain[i].backingStore = &backingchain[i + 1];
        else
            backingchain[i].backingStore = NULL;

        backingchain[i].relPath = NULL;
    }

    /* normal relative backing chain */
    backingchain[0].path = (char *) "/path/to/some/img";

    backingchain[1].path = (char *) "/path/to/some/asdf";
    backingchain[1].relPath = (char *) "asdf";

    backingchain[2].path = (char *) "/path/to/some/test";
    backingchain[2].relPath = (char *) "test";

    backingchain[3].path = (char *) "/path/to/some/blah";
    backingchain[3].relPath = (char *) "blah";

    /* ovirt's backing chain */
    backingchain[4].path = (char *) "/path/to/volume/image1";

    backingchain[5].path = (char *) "/path/to/volume/image2";
    backingchain[5].relPath = (char *) "../volume/image2";

    backingchain[6].path = (char *) "/path/to/volume/image3";
    backingchain[6].relPath = (char *) "../volume/image3";

    backingchain[7].path = (char *) "/path/to/volume/image4";
    backingchain[7].relPath = (char *) "../volume/image4";

    /* some arbitrarily crazy backing chains */
    backingchain[8].path = (char *) "/crazy/base/image";

    backingchain[9].path = (char *) "/crazy/base/directory/stuff/volumes/garbage/image2";
    backingchain[9].relPath = (char *) "directory/stuff/volumes/garbage/image2";

    backingchain[10].path = (char *) "/crazy/base/directory/image3";
    backingchain[10].relPath = (char *) "../../../image3";

    backingchain[11].path = (char *) "/crazy/base/blah/image4";
    backingchain[11].relPath = (char *) "../blah/image4";
}