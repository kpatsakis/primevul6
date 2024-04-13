mymain(void)
{
    int ret;
    struct testChainData data;
    struct testLookupData data2;
    struct testPathCanonicalizeData data3;
    struct testPathRelativeBacking data4;
    struct testBackingParseData data5;
    virStorageSourcePtr chain2; /* short for chain->backingStore */
    virStorageSourcePtr chain3; /* short for chain2->backingStore */
    g_autoptr(virCommand) cmd = NULL;
    g_autoptr(virStorageSource) chain = NULL;

    if (storageRegisterAll() < 0)
       return EXIT_FAILURE;

    /* Prep some files with qemu-img; if that is not found on PATH, or
     * if it lacks support for qcow2 and qed, skip this test.  */
    if ((ret = testPrepImages()) != 0)
        return ret;

#define TEST_ONE_CHAIN(start, format, flags, ...) \
    do { \
        size_t i; \
        memset(&data, 0, sizeof(data)); \
        data = (struct testChainData){ \
            start, format, { __VA_ARGS__ }, 0, flags, \
        }; \
        for (i = 0; i < G_N_ELEMENTS(data.files); i++) \
            if (data.files[i]) \
                data.nfiles++; \
        if (virTestRun(virTestCounterNext(), \
                       testStorageChain, &data) < 0) \
            ret = -1; \
    } while (0)

#define VIR_FLATTEN_2(...) __VA_ARGS__
#define VIR_FLATTEN_1(_1) VIR_FLATTEN_2 _1

#define TEST_CHAIN(path, format, chain, flags) \
    TEST_ONE_CHAIN(path, format, flags, VIR_FLATTEN_1(chain));

    /* The actual tests, in several groups. */
    virTestCounterReset("Storage backing chain ");

    /* Missing file */
    TEST_ONE_CHAIN("bogus", VIR_STORAGE_FILE_RAW, EXP_FAIL);

    /* Raw image, whether with right format or no specified format */
    testFileData raw = {
        .path = absraw,
        .type = VIR_STORAGE_TYPE_FILE,
        .format = VIR_STORAGE_FILE_RAW,
    };
    TEST_CHAIN(absraw, VIR_STORAGE_FILE_RAW, (&raw), EXP_PASS);
    TEST_CHAIN(absraw, VIR_STORAGE_FILE_AUTO, (&raw), EXP_PASS);

    /* Qcow2 file with relative raw backing, format provided */
    raw.pathRel = "raw";
    testFileData qcow2 = {
        .expBackingStoreRaw = "raw",
        .expCapacity = 1024,
        .path = absqcow2,
        .type = VIR_STORAGE_TYPE_FILE,
        .format = VIR_STORAGE_FILE_QCOW2,
    };
    testFileData qcow2_as_raw = {
        .path = absqcow2,
        .type = VIR_STORAGE_TYPE_FILE,
        .format = VIR_STORAGE_FILE_RAW,
    };
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_QCOW2, (&qcow2, &raw), EXP_PASS);
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_AUTO, (&qcow2_as_raw), EXP_PASS);

    /* Rewrite qcow2 file to use absolute backing name */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "raw", "-b", absraw, "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;
    qcow2.expBackingStoreRaw = absraw;
    raw.pathRel = NULL;

    /* Qcow2 file with raw as absolute backing, backing format provided */
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_QCOW2, (&qcow2, &raw), EXP_PASS);
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_AUTO, (&qcow2_as_raw), EXP_PASS);

    /* Wrapped file access */
    testFileData wrap = {
        .expBackingStoreRaw = absqcow2,
        .expCapacity = 1024,
        .path = abswrap,
        .type = VIR_STORAGE_TYPE_FILE,
        .format = VIR_STORAGE_FILE_QCOW2,
    };
    TEST_CHAIN(abswrap, VIR_STORAGE_FILE_QCOW2, (&wrap, &qcow2, &raw), EXP_PASS);

    /* Rewrite qcow2 and wrap file to omit backing file type */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-b", absraw, "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;

    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-b", absqcow2, "wrap", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;

    /* Qcow2 file with raw as absolute backing, backing format omitted */
    testFileData wrap_as_raw = {
        .expBackingStoreRaw = absqcow2,
        .expCapacity = 1024,
        .path = abswrap,
        .type = VIR_STORAGE_TYPE_FILE,
        .format = VIR_STORAGE_FILE_QCOW2,
    };
    TEST_CHAIN(abswrap, VIR_STORAGE_FILE_QCOW2,
               (&wrap_as_raw, &qcow2_as_raw), EXP_FAIL);

    /* Rewrite qcow2 to a missing backing file, with backing type */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "qcow2", "-b", datadir "/bogus",
                               "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;
    qcow2.expBackingStoreRaw = datadir "/bogus";

    /* Qcow2 file with missing backing file but specified type */
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_QCOW2, (&qcow2), EXP_FAIL);

    /* Rewrite qcow2 to a missing backing file, without backing type */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-b", datadir "/bogus", "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;

    /* Qcow2 file with missing backing file and no specified type */
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_QCOW2, (&qcow2), EXP_FAIL);

    /* Rewrite qcow2 to use an nbd: protocol as backend */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "raw", "-b", "nbd:example.org:6000:exportname=blah",
                               "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;
    qcow2.expBackingStoreRaw = "nbd:example.org:6000:exportname=blah";

    /* Qcow2 file with backing protocol instead of file */
    testFileData nbd = {
        .path = "blah",
        .type = VIR_STORAGE_TYPE_NETWORK,
        .format = VIR_STORAGE_FILE_RAW,
        .protocol = VIR_STORAGE_NET_PROTOCOL_NBD,
        .hostname = "example.org",
    };
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_QCOW2, (&qcow2, &nbd), EXP_PASS);

    /* Rewrite qcow2 to use an nbd: protocol as backend */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "raw", "-b", "nbd+tcp://example.org:6000/blah",
                               "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;
    qcow2.expBackingStoreRaw = "nbd+tcp://example.org:6000/blah";

    /* Qcow2 file with backing protocol instead of file */
    testFileData nbd2 = {
        .path = "blah",
        .type = VIR_STORAGE_TYPE_NETWORK,
        .format = VIR_STORAGE_FILE_RAW,
        .protocol = VIR_STORAGE_NET_PROTOCOL_NBD,
        .hostname = "example.org",
    };
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_QCOW2, (&qcow2, &nbd2), EXP_PASS);

    /* Rewrite qcow2 to use an nbd: protocol without path as backend */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "raw", "-b", "nbd://example.org",
                               "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;
    qcow2.expBackingStoreRaw = "nbd://example.org";

    nbd2.path = NULL;
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_QCOW2, (&qcow2, &nbd2), EXP_PASS);

    /* qed file */
    testFileData qed = {
        .expBackingStoreRaw = absraw,
        .expCapacity = 1024,
        .path = absqed,
        .type = VIR_STORAGE_TYPE_FILE,
        .format = VIR_STORAGE_FILE_QED,
    };
    testFileData qed_as_raw = {
        .path = absqed,
        .type = VIR_STORAGE_TYPE_FILE,
        .format = VIR_STORAGE_FILE_RAW,
    };
    TEST_CHAIN(absqed, VIR_STORAGE_FILE_QED, (&qed, &raw), EXP_PASS);
    TEST_CHAIN(absqed, VIR_STORAGE_FILE_AUTO, (&qed_as_raw), EXP_PASS);

    /* directory */
    testFileData dir = {
        .path = absdir,
        .type = VIR_STORAGE_TYPE_DIR,
        .format = VIR_STORAGE_FILE_DIR,
    };
    testFileData dir_as_raw = {
        .path = absdir,
        .type = VIR_STORAGE_TYPE_DIR,
        .format = VIR_STORAGE_FILE_RAW,
    };
    TEST_CHAIN(absdir, VIR_STORAGE_FILE_RAW, (&dir_as_raw), EXP_PASS);
    TEST_CHAIN(absdir, VIR_STORAGE_FILE_NONE, (&dir), EXP_PASS);
    TEST_CHAIN(absdir, VIR_STORAGE_FILE_DIR, (&dir), EXP_PASS);

#ifdef HAVE_SYMLINK
    /* Rewrite qcow2 and wrap file to use backing names relative to a
     * symlink from a different directory */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "raw", "-b", "../raw", "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;

    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "qcow2", "-b", "../sub/link1", "wrap",
                               NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;

    /* Behavior of symlinks to qcow2 with relative backing files */
    testFileData link1 = {
        .expBackingStoreRaw = "../raw",
        .expCapacity = 1024,
        .pathRel = "../sub/link1",
        .path = datadir "/sub/../sub/link1",
        .type = VIR_STORAGE_TYPE_FILE,
        .format = VIR_STORAGE_FILE_QCOW2,
    };
    testFileData link2 = {
        .expBackingStoreRaw = "../sub/link1",
        .expCapacity = 1024,
        .path = abslink2,
        .type = VIR_STORAGE_TYPE_FILE,
        .format = VIR_STORAGE_FILE_QCOW2,
    };

    raw.path = datadir "/sub/../sub/../raw";
    raw.pathRel = "../raw";
    TEST_CHAIN(abslink2, VIR_STORAGE_FILE_QCOW2,
               (&link2, &link1, &raw), EXP_PASS);
#endif

    /* Rewrite qcow2 to be a self-referential loop */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "qcow2", "-b", "qcow2", "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;
    qcow2.expBackingStoreRaw = "qcow2";

    /* Behavior of an infinite loop chain */
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_QCOW2, (&qcow2), EXP_FAIL);

    /* Rewrite wrap and qcow2 to be mutually-referential loop */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "qcow2", "-b", "wrap", "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;

    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "qcow2", "-b", absqcow2, "wrap", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;
    qcow2.expBackingStoreRaw = "wrap";

    /* Behavior of an infinite loop chain */
    TEST_CHAIN(abswrap, VIR_STORAGE_FILE_QCOW2, (&wrap, &qcow2), EXP_FAIL);

    /* Rewrite qcow2 to use an rbd: protocol as backend */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "raw", "-b", "rbd:testshare",
                               "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;
    qcow2.expBackingStoreRaw = "rbd:testshare";

    /* Qcow2 file with backing protocol instead of file */
    testFileData rbd1 = {
        .path = "testshare",
        .type = VIR_STORAGE_TYPE_NETWORK,
        .format = VIR_STORAGE_FILE_RAW,
        .protocol = VIR_STORAGE_NET_PROTOCOL_RBD,
    };
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_QCOW2, (&qcow2, &rbd1), EXP_PASS);

    /* Rewrite qcow2 to use an rbd: protocol as backend */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "raw", "-b", "rbd:testshare:id=asdf:mon_host=example.com",
                               "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;
    qcow2.expBackingStoreRaw = "rbd:testshare:id=asdf:mon_host=example.com";

    /* Qcow2 file with backing protocol instead of file */
    testFileData rbd2 = {
        .path = "testshare",
        .type = VIR_STORAGE_TYPE_NETWORK,
        .format = VIR_STORAGE_FILE_RAW,
        .protocol = VIR_STORAGE_NET_PROTOCOL_RBD,
        .secret = "asdf",
        .hostname = "example.com",
    };
    TEST_CHAIN(absqcow2, VIR_STORAGE_FILE_QCOW2, (&qcow2, &rbd2), EXP_PASS);


    /* Rewrite wrap and qcow2 back to 3-deep chain, absolute backing */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "raw", "-b", absraw, "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;

    /* Test behavior of chain lookups, absolute backing from relative start */
    chain = testStorageFileGetMetadata("wrap", VIR_STORAGE_FILE_QCOW2,
                                       -1, -1);
    if (!chain) {
        ret = -1;
        goto cleanup;
    }
    chain2 = chain->backingStore;
    chain3 = chain2->backingStore;

#define TEST_LOOKUP_TARGET(id, target, from, name, index, result, \
                           meta, parent) \
    do { \
        data2 = (struct testLookupData){ \
            chain, target, from, name, index, \
            result, meta, parent, }; \
        if (virTestRun("Chain lookup " #id, \
                       testStorageLookup, &data2) < 0) \
            ret = -1; \
    } while (0)
#define TEST_LOOKUP(id, from, name, result, meta, parent) \
    TEST_LOOKUP_TARGET(id, NULL, from, name, 0, result, meta, parent)

    TEST_LOOKUP(0, NULL, "bogus", NULL, NULL, NULL);
    TEST_LOOKUP(1, chain, "bogus", NULL, NULL, NULL);
    TEST_LOOKUP(2, NULL, "wrap", chain->path, chain, NULL);
    TEST_LOOKUP(3, chain, "wrap", NULL, NULL, NULL);
    TEST_LOOKUP(4, chain2, "wrap", NULL, NULL, NULL);
    TEST_LOOKUP(5, NULL, abswrap, chain->path, chain, NULL);
    TEST_LOOKUP(6, chain, abswrap, NULL, NULL, NULL);
    TEST_LOOKUP(7, chain2, abswrap, NULL, NULL, NULL);
    TEST_LOOKUP(8, NULL, "qcow2", chain2->path, chain2, chain);
    TEST_LOOKUP(9, chain, "qcow2", chain2->path, chain2, chain);
    TEST_LOOKUP(10, chain2, "qcow2", NULL, NULL, NULL);
    TEST_LOOKUP(11, chain3, "qcow2", NULL, NULL, NULL);
    TEST_LOOKUP(12, NULL, absqcow2, chain2->path, chain2, chain);
    TEST_LOOKUP(13, chain, absqcow2, chain2->path, chain2, chain);
    TEST_LOOKUP(14, chain2, absqcow2, NULL, NULL, NULL);
    TEST_LOOKUP(15, chain3, absqcow2, NULL, NULL, NULL);
    TEST_LOOKUP(16, NULL, "raw", chain3->path, chain3, chain2);
    TEST_LOOKUP(17, chain, "raw", chain3->path, chain3, chain2);
    TEST_LOOKUP(18, chain2, "raw", chain3->path, chain3, chain2);
    TEST_LOOKUP(19, chain3, "raw", NULL, NULL, NULL);
    TEST_LOOKUP(20, NULL, absraw, chain3->path, chain3, chain2);
    TEST_LOOKUP(21, chain, absraw, chain3->path, chain3, chain2);
    TEST_LOOKUP(22, chain2, absraw, chain3->path, chain3, chain2);
    TEST_LOOKUP(23, chain3, absraw, NULL, NULL, NULL);
    TEST_LOOKUP(24, NULL, NULL, chain3->path, chain3, chain2);
    TEST_LOOKUP(25, chain, NULL, chain3->path, chain3, chain2);
    TEST_LOOKUP(26, chain2, NULL, chain3->path, chain3, chain2);
    TEST_LOOKUP(27, chain3, NULL, NULL, NULL, NULL);

    /* Rewrite wrap and qcow2 back to 3-deep chain, relative backing */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "raw", "-b", "raw", "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;

    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "qcow2", "-b", "qcow2", "wrap", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;

    /* Test behavior of chain lookups, relative backing from absolute start */
    virObjectUnref(chain);
    chain = testStorageFileGetMetadata(abswrap, VIR_STORAGE_FILE_QCOW2, -1, -1);
    if (!chain) {
        ret = -1;
        goto cleanup;
    }
    chain2 = chain->backingStore;
    chain3 = chain2->backingStore;

    TEST_LOOKUP(28, NULL, "bogus", NULL, NULL, NULL);
    TEST_LOOKUP(29, chain, "bogus", NULL, NULL, NULL);
    TEST_LOOKUP(30, NULL, "wrap", chain->path, chain, NULL);
    TEST_LOOKUP(31, chain, "wrap", NULL, NULL, NULL);
    TEST_LOOKUP(32, chain2, "wrap", NULL, NULL, NULL);
    TEST_LOOKUP(33, NULL, abswrap, chain->path, chain, NULL);
    TEST_LOOKUP(34, chain, abswrap, NULL, NULL, NULL);
    TEST_LOOKUP(35, chain2, abswrap, NULL, NULL, NULL);
    TEST_LOOKUP(36, NULL, "qcow2", chain2->path, chain2, chain);
    TEST_LOOKUP(37, chain, "qcow2", chain2->path, chain2, chain);
    TEST_LOOKUP(38, chain2, "qcow2", NULL, NULL, NULL);
    TEST_LOOKUP(39, chain3, "qcow2", NULL, NULL, NULL);
    TEST_LOOKUP(40, NULL, absqcow2, chain2->path, chain2, chain);
    TEST_LOOKUP(41, chain, absqcow2, chain2->path, chain2, chain);
    TEST_LOOKUP(42, chain2, absqcow2, NULL, NULL, NULL);
    TEST_LOOKUP(43, chain3, absqcow2, NULL, NULL, NULL);
    TEST_LOOKUP(44, NULL, "raw", chain3->path, chain3, chain2);
    TEST_LOOKUP(45, chain, "raw", chain3->path, chain3, chain2);
    TEST_LOOKUP(46, chain2, "raw", chain3->path, chain3, chain2);
    TEST_LOOKUP(47, chain3, "raw", NULL, NULL, NULL);
    TEST_LOOKUP(48, NULL, absraw, chain3->path, chain3, chain2);
    TEST_LOOKUP(49, chain, absraw, chain3->path, chain3, chain2);
    TEST_LOOKUP(50, chain2, absraw, chain3->path, chain3, chain2);
    TEST_LOOKUP(51, chain3, absraw, NULL, NULL, NULL);
    TEST_LOOKUP(52, NULL, NULL, chain3->path, chain3, chain2);
    TEST_LOOKUP(53, chain, NULL, chain3->path, chain3, chain2);
    TEST_LOOKUP(54, chain2, NULL, chain3->path, chain3, chain2);
    TEST_LOOKUP(55, chain3, NULL, NULL, NULL, NULL);

    /* Use link to wrap with cross-directory relative backing */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "qcow2", "-b", "../qcow2", "wrap", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        ret = -1;

    /* Test behavior of chain lookups, relative backing */
    virObjectUnref(chain);
    chain = testStorageFileGetMetadata("sub/link2", VIR_STORAGE_FILE_QCOW2,
                                       -1, -1);
    if (!chain) {
        ret = -1;
        goto cleanup;
    }
    chain2 = chain->backingStore;
    chain3 = chain2->backingStore;

    TEST_LOOKUP(56, NULL, "bogus", NULL, NULL, NULL);
    TEST_LOOKUP(57, NULL, "sub/link2", chain->path, chain, NULL);
    TEST_LOOKUP(58, NULL, "wrap", chain->path, chain, NULL);
    TEST_LOOKUP(59, NULL, abswrap, chain->path, chain, NULL);
    TEST_LOOKUP(60, NULL, "../qcow2", chain2->path, chain2, chain);
    TEST_LOOKUP(61, NULL, "qcow2", NULL, NULL, NULL);
    TEST_LOOKUP(62, NULL, absqcow2, chain2->path, chain2, chain);
    TEST_LOOKUP(63, NULL, "raw", chain3->path, chain3, chain2);
    TEST_LOOKUP(64, NULL, absraw, chain3->path, chain3, chain2);
    TEST_LOOKUP(65, NULL, NULL, chain3->path, chain3, chain2);

    TEST_LOOKUP_TARGET(66, "vda", NULL, "bogus[1]", 0, NULL, NULL, NULL);
    TEST_LOOKUP_TARGET(67, "vda", NULL, "vda[-1]", 0, NULL, NULL, NULL);
    TEST_LOOKUP_TARGET(68, "vda", NULL, "vda[1][1]", 0, NULL, NULL, NULL);
    TEST_LOOKUP_TARGET(69, "vda", NULL, "wrap", 0, chain->path, chain, NULL);
    TEST_LOOKUP_TARGET(70, "vda", chain, "wrap", 0, NULL, NULL, NULL);
    TEST_LOOKUP_TARGET(71, "vda", chain2, "wrap", 0, NULL, NULL, NULL);
    TEST_LOOKUP_TARGET(72, "vda", NULL, "vda[0]", 0, NULL, NULL, NULL);
    TEST_LOOKUP_TARGET(73, "vda", NULL, "vda[1]", 1, chain2->path, chain2, chain);
    TEST_LOOKUP_TARGET(74, "vda", chain, "vda[1]", 1, chain2->path, chain2, chain);
    TEST_LOOKUP_TARGET(75, "vda", chain2, "vda[1]", 1, NULL, NULL, NULL);
    TEST_LOOKUP_TARGET(76, "vda", chain3, "vda[1]", 1, NULL, NULL, NULL);
    TEST_LOOKUP_TARGET(77, "vda", NULL, "vda[2]", 2, chain3->path, chain3, chain2);
    TEST_LOOKUP_TARGET(78, "vda", chain, "vda[2]", 2, chain3->path, chain3, chain2);
    TEST_LOOKUP_TARGET(79, "vda", chain2, "vda[2]", 2, chain3->path, chain3, chain2);
    TEST_LOOKUP_TARGET(80, "vda", chain3, "vda[2]", 2, NULL, NULL, NULL);
    TEST_LOOKUP_TARGET(81, "vda", NULL, "vda[3]", 3, NULL, NULL, NULL);

#define TEST_PATH_CANONICALIZE(id, PATH, EXPECT) \
    do { \
        data3.path = PATH; \
        data3.expect = EXPECT; \
        if (virTestRun("Path canonicalize " #id, \
                       testPathCanonicalize, &data3) < 0) \
            ret = -1; \
    } while (0)

    TEST_PATH_CANONICALIZE(1, "/", "/");
    TEST_PATH_CANONICALIZE(2, "/path", "/path");
    TEST_PATH_CANONICALIZE(3, "/path/to/blah", "/path/to/blah");
    TEST_PATH_CANONICALIZE(4, "/path/", "/path");
    TEST_PATH_CANONICALIZE(5, "///////", "/");
    TEST_PATH_CANONICALIZE(6, "//", "//");
    TEST_PATH_CANONICALIZE(7, "", "");
    TEST_PATH_CANONICALIZE(8, ".", ".");
    TEST_PATH_CANONICALIZE(9, "../", "..");
    TEST_PATH_CANONICALIZE(10, "../../", "../..");
    TEST_PATH_CANONICALIZE(11, "../../blah", "../../blah");
    TEST_PATH_CANONICALIZE(12, "/./././blah", "/blah");
    TEST_PATH_CANONICALIZE(13, ".././../././../blah", "../../../blah");
    TEST_PATH_CANONICALIZE(14, "/././", "/");
    TEST_PATH_CANONICALIZE(15, "./././", ".");
    TEST_PATH_CANONICALIZE(16, "blah/../foo", "foo");
    TEST_PATH_CANONICALIZE(17, "foo/bar/../blah", "foo/blah");
    TEST_PATH_CANONICALIZE(18, "foo/bar/.././blah", "foo/blah");
    TEST_PATH_CANONICALIZE(19, "/path/to/foo/bar/../../../../../../../../baz", "/baz");
    TEST_PATH_CANONICALIZE(20, "path/to/foo/bar/../../../../../../../../baz", "../../../../baz");
    TEST_PATH_CANONICALIZE(21, "path/to/foo/bar", "path/to/foo/bar");
    TEST_PATH_CANONICALIZE(22, "//foo//bar", "//foo/bar");
    TEST_PATH_CANONICALIZE(23, "/bar//foo", "/bar/foo");
    TEST_PATH_CANONICALIZE(24, "//../blah", "//blah");

    /* test paths with symlinks */
    TEST_PATH_CANONICALIZE(25, "/path/blah", "/other/path/huzah");
    TEST_PATH_CANONICALIZE(26, "/path/to/relative/symlink", "/path/actual/file");
    TEST_PATH_CANONICALIZE(27, "/path/to/relative/symlink/blah", "/path/actual/file/blah");
    TEST_PATH_CANONICALIZE(28, "/path/blah/yippee", "/other/path/huzah/yippee");
    TEST_PATH_CANONICALIZE(29, "/cycle", NULL);
    TEST_PATH_CANONICALIZE(30, "/cycle2/link", NULL);
    TEST_PATH_CANONICALIZE(31, "///", "/");

#define TEST_RELATIVE_BACKING(id, TOP, BASE, EXPECT) \
    do { \
        data4.top = &TOP; \
        data4.base = &BASE; \
        data4.expect = EXPECT; \
        if (virTestRun("Path relative resolve " #id, \
                       testPathRelative, &data4) < 0) \
            ret = -1; \
    } while (0)

    testPathRelativePrepare();

    /* few negative tests first */

    /* a non-relative image is in the backing chain span */
    TEST_RELATIVE_BACKING(1, backingchain[0], backingchain[1], NULL);
    TEST_RELATIVE_BACKING(2, backingchain[0], backingchain[2], NULL);
    TEST_RELATIVE_BACKING(3, backingchain[0], backingchain[3], NULL);
    TEST_RELATIVE_BACKING(4, backingchain[1], backingchain[5], NULL);

    /* image is not in chain (specified backwards) */
    TEST_RELATIVE_BACKING(5, backingchain[2], backingchain[1], NULL);

    /* positive tests */
    TEST_RELATIVE_BACKING(6, backingchain[1], backingchain[1], "asdf");
    TEST_RELATIVE_BACKING(7, backingchain[1], backingchain[2], "test");
    TEST_RELATIVE_BACKING(8, backingchain[1], backingchain[3], "blah");
    TEST_RELATIVE_BACKING(9, backingchain[2], backingchain[2], "test");
    TEST_RELATIVE_BACKING(10, backingchain[2], backingchain[3], "blah");
    TEST_RELATIVE_BACKING(11, backingchain[3], backingchain[3], "blah");

    /* oVirt spelling */
    TEST_RELATIVE_BACKING(12, backingchain[5], backingchain[5], "../volume/image2");
    TEST_RELATIVE_BACKING(13, backingchain[5], backingchain[6], "../volume/../volume/image3");
    TEST_RELATIVE_BACKING(14, backingchain[5], backingchain[7], "../volume/../volume/../volume/image4");
    TEST_RELATIVE_BACKING(15, backingchain[6], backingchain[6], "../volume/image3");
    TEST_RELATIVE_BACKING(16, backingchain[6], backingchain[7], "../volume/../volume/image4");
    TEST_RELATIVE_BACKING(17, backingchain[7], backingchain[7], "../volume/image4");

    /* crazy spellings */
    TEST_RELATIVE_BACKING(17, backingchain[9], backingchain[9], "directory/stuff/volumes/garbage/image2");
    TEST_RELATIVE_BACKING(18, backingchain[9], backingchain[10], "directory/stuff/volumes/garbage/../../../image3");
    TEST_RELATIVE_BACKING(19, backingchain[9], backingchain[11], "directory/stuff/volumes/garbage/../../../../blah/image4");
    TEST_RELATIVE_BACKING(20, backingchain[10], backingchain[10], "../../../image3");
    TEST_RELATIVE_BACKING(21, backingchain[10], backingchain[11], "../../../../blah/image4");
    TEST_RELATIVE_BACKING(22, backingchain[11], backingchain[11], "../blah/image4");


    virTestCounterReset("Backing store parse ");

#define TEST_BACKING_PARSE_FULL(bck, xml, rc) \
    do { \
        data5.backing = bck; \
        data5.expect = xml; \
        data5.rv = rc; \
        if (virTestRun(virTestCounterNext(), \
                       testBackingParse, &data5) < 0) \
            ret = -1; \
    } while (0)

#define TEST_BACKING_PARSE(bck, xml) \
    TEST_BACKING_PARSE_FULL(bck, xml, 0)

    TEST_BACKING_PARSE("path", "<source file='path'/>\n");
    TEST_BACKING_PARSE("fat:/somedir", "<source dir='/somedir'/>\n");
    TEST_BACKING_PARSE("://", NULL);
    TEST_BACKING_PARSE("http://example.com",
                       "<source protocol='http' name=''>\n"
                       "  <host name='example.com' port='80'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("http://example.com/",
                       "<source protocol='http' name=''>\n"
                       "  <host name='example.com' port='80'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("http://example.com/file",
                       "<source protocol='http' name='file'>\n"
                       "  <host name='example.com' port='80'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE_FULL("http://user:pass@example.com/file",
                            "<source protocol='http' name='file'>\n"
                            "  <host name='example.com' port='80'/>\n"
                            "</source>\n", 1);
    TEST_BACKING_PARSE("rbd:testshare:id=asdf:mon_host=example.com",
                       "<source protocol='rbd' name='testshare'>\n"
                       "  <host name='example.com'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd:example.org:6000:exportname=blah",
                       "<source protocol='nbd' name='blah'>\n"
                       "  <host name='example.org' port='6000'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd:example.org:6000:exportname=:",
                       "<source protocol='nbd' name=':'>\n"
                       "  <host name='example.org' port='6000'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd:example.org:6000:exportname=:test",
                       "<source protocol='nbd' name=':test'>\n"
                       "  <host name='example.org' port='6000'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd:unix:/tmp/sock:exportname=/",
                       "<source protocol='nbd' name='/'>\n"
                       "  <host transport='unix' socket='/tmp/sock'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd://example.org:1234",
                       "<source protocol='nbd'>\n"
                       "  <host name='example.org' port='1234'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd://example.org:1234/",
                       "<source protocol='nbd'>\n"
                       "  <host name='example.org' port='1234'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd://example.org:1234/exportname",
                       "<source protocol='nbd' name='exportname'>\n"
                       "  <host name='example.org' port='1234'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd+unix://?socket=/tmp/sock",
                       "<source protocol='nbd'>\n"
                       "  <host transport='unix' socket='/tmp/sock'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd+unix:///?socket=/tmp/sock",
                       "<source protocol='nbd'>\n"
                       "  <host transport='unix' socket='/tmp/sock'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd+unix:////?socket=/tmp/sock",
                       "<source protocol='nbd' name='/'>\n"
                       "  <host transport='unix' socket='/tmp/sock'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd+unix:///exp?socket=/tmp/sock",
                       "<source protocol='nbd' name='exp'>\n"
                       "  <host transport='unix' socket='/tmp/sock'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("nbd+unix:////exp?socket=/tmp/sock",
                       "<source protocol='nbd' name='/exp'>\n"
                       "  <host transport='unix' socket='/tmp/sock'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE_FULL("iscsi://testuser:testpass@example.org:1234/exportname",
                            "<source protocol='iscsi' name='exportname'>\n"
                            "  <host name='example.org' port='1234'/>\n"
                            "</source>\n", 1);

#ifdef WITH_YAJL
    TEST_BACKING_PARSE("json:", NULL);
    TEST_BACKING_PARSE("json:asdgsdfg", NULL);
    TEST_BACKING_PARSE("json:{}", NULL);
    TEST_BACKING_PARSE("json: { \"file.driver\":\"blah\"}", NULL);
    TEST_BACKING_PARSE("json:{\"file.driver\":\"file\"}", NULL);
    TEST_BACKING_PARSE("json:{\"file.driver\":\"file\", "
                             "\"file.filename\":\"/path/to/file\"}",
                       "<source file='/path/to/file'/>\n");
    TEST_BACKING_PARSE("json:{\"file.driver\":\"file\", "
                             "\"filename\":\"/path/to/file\"}", NULL);
    TEST_BACKING_PARSE("json:{\"file\" : { \"driver\":\"file\","
                                          "\"filename\":\"/path/to/file\""
                                        "}"
                            "}",
                       "<source file='/path/to/file'/>\n");
    TEST_BACKING_PARSE("json:{\"driver\":\"file\","
                             "\"filename\":\"/path/to/file\""
                            "}",
                       "<source file='/path/to/file'/>\n");
    TEST_BACKING_PARSE("json:{\"file.driver\":\"host_device\", "
                             "\"file.filename\":\"/path/to/dev\"}",
                       "<source dev='/path/to/dev'/>\n");
    TEST_BACKING_PARSE("json:{\"file.driver\":\"host_cdrom\", "
                             "\"file.filename\":\"/path/to/cdrom\"}",
                       "<source dev='/path/to/cdrom'/>\n");
    TEST_BACKING_PARSE("json:{\"file.driver\":\"http\", "
                             "\"file.url\":\"http://example.com/file\"}",
                       "<source protocol='http' name='file'>\n"
                       "  <host name='example.com' port='80'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{ \"driver\":\"http\","
                                        "\"url\":\"http://example.com/file\""
                                      "}"
                            "}",
                       "<source protocol='http' name='file'>\n"
                       "  <host name='example.com' port='80'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file.driver\":\"ftp\", "
                             "\"file.url\":\"http://example.com/file\"}",
                       NULL);
    TEST_BACKING_PARSE("json:{\"file.driver\":\"gluster\", "
                             "\"file.filename\":\"gluster://example.com/vol/file\"}",
                       "<source protocol='gluster' name='vol/file'>\n"
                       "  <host name='example.com' port='24007'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"gluster\","
                                       "\"volume\":\"testvol\","
                                       "\"path\":\"img.qcow2\","
                                       "\"server\":[ { \"type\":\"tcp\","
                                                      "\"host\":\"example.com\","
                                                      "\"port\":\"1234\""
                                                    "},"
                                                    "{ \"type\":\"unix\","
                                                      "\"socket\":\"/path/socket\""
                                                    "},"
                                                    "{ \"type\":\"tcp\","
                                                      "\"host\":\"example.com\""
                                                    "}"
                                                  "]"
                                      "}"
                             "}",
                        "<source protocol='gluster' name='testvol/img.qcow2'>\n"
                        "  <host name='example.com' port='1234'/>\n"
                        "  <host transport='unix' socket='/path/socket'/>\n"
                        "  <host name='example.com' port='24007'/>\n"
                        "</source>\n");
    TEST_BACKING_PARSE("json:{\"file.driver\":\"gluster\","
                             "\"file.volume\":\"testvol\","
                             "\"file.path\":\"img.qcow2\","
                             "\"file.server\":[ { \"type\":\"tcp\","
                                                 "\"host\":\"example.com\","
                                                 "\"port\":\"1234\""
                                               "},"
                                               "{ \"type\":\"unix\","
                                                 "\"socket\":\"/path/socket\""
                                               "},"
                                               "{ \"type\":\"inet\","
                                                 "\"host\":\"example.com\""
                                               "}"
                                             "]"
                            "}",
                        "<source protocol='gluster' name='testvol/img.qcow2'>\n"
                        "  <host name='example.com' port='1234'/>\n"
                        "  <host transport='unix' socket='/path/socket'/>\n"
                        "  <host name='example.com' port='24007'/>\n"
                        "</source>\n");
    TEST_BACKING_PARSE("json:{\"driver\": \"raw\","
                             "\"file\": {\"server.0.host\": \"A.A.A.A\","
                                        "\"server.1.host\": \"B.B.B.B\","
                                        "\"server.2.host\": \"C.C.C.C\","
                                        "\"driver\": \"gluster\","
                                        "\"path\": \"raw\","
                                        "\"server.0.type\": \"tcp\","
                                        "\"server.1.type\": \"tcp\","
                                        "\"server.2.type\": \"tcp\","
                                        "\"server.0.port\": \"24007\","
                                        "\"server.1.port\": \"24007\","
                                        "\"server.2.port\": \"24007\","
                                        "\"volume\": \"vol1\"}}",
                       "<source protocol='gluster' name='vol1/raw'>\n"
                       "  <host name='A.A.A.A' port='24007'/>\n"
                       "  <host name='B.B.B.B' port='24007'/>\n"
                       "  <host name='C.C.C.C' port='24007'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"nbd\","
                                       "\"path\":\"/path/to/socket\""
                                      "}"
                            "}",
                       "<source protocol='nbd'>\n"
                       "  <host transport='unix' socket='/path/to/socket'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"driver\":\"nbd\","
                             "\"path\":\"/path/to/socket\""
                            "}",
                       "<source protocol='nbd'>\n"
                       "  <host transport='unix' socket='/path/to/socket'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file.driver\":\"nbd\","
                             "\"file.path\":\"/path/to/socket\""
                            "}",
                       "<source protocol='nbd'>\n"
                       "  <host transport='unix' socket='/path/to/socket'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"nbd\","
                                       "\"export\":\"blah\","
                                       "\"host\":\"example.org\","
                                       "\"port\":\"6000\""
                                      "}"
                            "}",
                       "<source protocol='nbd' name='blah'>\n"
                       "  <host name='example.org' port='6000'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file.driver\":\"nbd\","
                             "\"file.export\":\"blah\","
                             "\"file.host\":\"example.org\","
                             "\"file.port\":\"6000\""
                            "}",
                       "<source protocol='nbd' name='blah'>\n"
                       "  <host name='example.org' port='6000'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"nbd\","
                                       "\"export\":\"blah\","
                                       "\"server\": { \"type\":\"inet\","
                                                     "\"host\":\"example.org\","
                                                     "\"port\":\"6000\""
                                                   "}"
                                      "}"
                            "}",
                       "<source protocol='nbd' name='blah'>\n"
                       "  <host name='example.org' port='6000'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"nbd\","
                                       "\"server\": { \"type\":\"unix\","
                                                     "\"path\":\"/path/socket\""
                                                   "}"
                                      "}"
                            "}",
                       "<source protocol='nbd'>\n"
                       "  <host transport='unix' socket='/path/socket'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"ssh\","
                                       "\"host\":\"example.org\","
                                       "\"port\":\"6000\","
                                       "\"path\":\"blah\","
                                       "\"user\":\"user\""
                                      "}"
                            "}",
                       "<source protocol='ssh' name='blah'>\n"
                       "  <host name='example.org' port='6000'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file.driver\":\"ssh\","
                             "\"file.host\":\"example.org\","
                             "\"file.port\":\"6000\","
                             "\"file.path\":\"blah\","
                             "\"file.user\":\"user\""
                            "}",
                       "<source protocol='ssh' name='blah'>\n"
                       "  <host name='example.org' port='6000'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"ssh\","
                                       "\"path\":\"blah\","
                                       "\"server\":{ \"host\":\"example.org\","
                                                    "\"port\":\"6000\""
                                                  "},"
                                       "\"user\":\"user\""
                                      "}"
                            "}",
                       "<source protocol='ssh' name='blah'>\n"
                       "  <host name='example.org' port='6000'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file.driver\":\"rbd\","
                             "\"file.filename\":\"rbd:testshare:id=asdf:mon_host=example.com\""
                            "}",
                       "<source protocol='rbd' name='testshare'>\n"
                       "  <host name='example.com'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"rbd\","
                                       "\"image\":\"test\","
                                       "\"pool\":\"libvirt\","
                                       "\"conf\":\"/path/to/conf\","
                                       "\"snapshot\":\"snapshotname\","
                                       "\"server\":[ {\"host\":\"example.com\","
                                                      "\"port\":\"1234\""
                                                    "},"
                                                    "{\"host\":\"example2.com\""
                                                    "}"
                                                  "]"
                                      "}"
                             "}",
                        "<source protocol='rbd' name='libvirt/test'>\n"
                        "  <host name='example.com' port='1234'/>\n"
                        "  <host name='example2.com'/>\n"
                        "  <snapshot name='snapshotname'/>\n"
                        "  <config file='/path/to/conf'/>\n"
                        "</source>\n");
    TEST_BACKING_PARSE("json:{ \"file\": { "
                                "\"driver\": \"raw\","
                                "\"file\": {"
                                    "\"driver\": \"file\","
                                    "\"filename\": \"/path/to/file\" } } }",
                       "<source file='/path/to/file'/>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"iscsi\","
                                       "\"transport\":\"tcp\","
                                       "\"portal\":\"test.org\","
                                       "\"target\":\"iqn.2016-12.com.virttest:emulated-iscsi-noauth.target\""
                                      "}"
                            "}",
                       "<source protocol='iscsi' name='iqn.2016-12.com.virttest:emulated-iscsi-noauth.target/0'>\n"
                       "  <host name='test.org' port='3260'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE_FULL("json:{\"file\":{\"driver\":\"iscsi\","
                                            "\"transport\":\"tcp\","
                                            "\"portal\":\"test.org\","
                                            "\"user\":\"testuser\","
                                            "\"target\":\"iqn.2016-12.com.virttest:emulated-iscsi-auth.target\""
                                            "}"
                            "}",
                       "<source protocol='iscsi' name='iqn.2016-12.com.virttest:emulated-iscsi-auth.target/0'>\n"
                       "  <host name='test.org' port='3260'/>\n"
                       "</source>\n", 1);
    TEST_BACKING_PARSE_FULL("json:{\"file\":{\"driver\":\"iscsi\","
                                            "\"transport\":\"tcp\","
                                            "\"portal\":\"test.org\","
                                            "\"password\":\"testpass\","
                                            "\"target\":\"iqn.2016-12.com.virttest:emulated-iscsi-auth.target\""
                                            "}"
                            "}",
                       "<source protocol='iscsi' name='iqn.2016-12.com.virttest:emulated-iscsi-auth.target/0'>\n"
                       "  <host name='test.org' port='3260'/>\n"
                       "</source>\n", 1);
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"iscsi\","
                                       "\"transport\":\"tcp\","
                                       "\"portal\":\"test.org:1234\","
                                       "\"target\":\"iqn.2016-12.com.virttest:emulated-iscsi-noauth.target\","
                                       "\"lun\":\"6\""
                                      "}"
                            "}",
                       "<source protocol='iscsi' name='iqn.2016-12.com.virttest:emulated-iscsi-noauth.target/6'>\n"
                       "  <host name='test.org' port='1234'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"iscsi\","
                                       "\"transport\":\"tcp\","
                                       "\"portal\":\"[2001::0]:1234\","
                                       "\"target\":\"iqn.2016-12.com.virttest:emulated-iscsi-noauth.target\","
                                       "\"lun\":6"
                                      "}"
                            "}",
                       "<source protocol='iscsi' name='iqn.2016-12.com.virttest:emulated-iscsi-noauth.target/6'>\n"
                       "  <host name='[2001::0]' port='1234'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"iscsi\","
                                       "\"transport\":\"tcp\","
                                       "\"portal\":\"[2001::0]\","
                                       "\"target\":\"iqn.2016-12.com.virttest:emulated-iscsi-noauth.target\","
                                       "\"lun\":6"
                                      "}"
                            "}",
                       "<source protocol='iscsi' name='iqn.2016-12.com.virttest:emulated-iscsi-noauth.target/6'>\n"
                       "  <host name='[2001::0]' port='3260'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"sheepdog\","
                                       "\"vdi\":\"test\","
                                       "\"server\":{ \"type\":\"inet\","
                                                    "\"host\":\"example.com\","
                                                    "\"port\":\"321\""
                                                  "}"
                                      "}"
                            "}",
                       "<source protocol='sheepdog' name='test'>\n"
                       "  <host name='example.com' port='321'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"driver\": \"raw\","
                             "\"file\": {\"server.host\": \"10.10.10.10\","
                                        "\"server.port\": \"7000\","
                                        "\"tag\": \"\","
                                        "\"driver\": \"sheepdog\","
                                        "\"server.type\": \"inet\","
                                        "\"vdi\": \"Alice\"}}",
                       "<source protocol='sheepdog' name='Alice'>\n"
                       "  <host name='10.10.10.10' port='7000'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE("json:{\"file\":{\"driver\":\"vxhs\","
                                       "\"vdisk-id\":\"c6718f6b-0401-441d-a8c3-1f0064d75ee0\","
                                       "\"server\": {  \"host\":\"example.com\","
                                                      "\"port\":\"9999\""
                                                   "}"
                                      "}"
                            "}",
                       "<source protocol='vxhs' name='c6718f6b-0401-441d-a8c3-1f0064d75ee0'>\n"
                       "  <host name='example.com' port='9999'/>\n"
                       "</source>\n");
    TEST_BACKING_PARSE_FULL("json:{ \"driver\": \"raw\","
                                    "\"offset\": 10752,"
                                    "\"size\": 4063232,"
                                    "\"file\": { \"driver\": \"file\","
                                                "\"filename\": \"/tmp/testfle\""
                                              "}"
                                  "}",
                            "<source file='/tmp/testfle'>\n"
                            "  <slices>\n"
                            "    <slice type='storage' offset='10752' size='4063232'/>\n"
                            "  </slices>\n"
                            "</source>\n", 0);

    TEST_BACKING_PARSE_FULL("json:{ \"file.cookie\": \"vmware_soap_session=\\\"0c8db85112873a79b7ef74f294cb70ef7f\\\"\","
                                   "\"file.sslverify\": false,"
                                   "\"file.driver\": \"https\","
                                   "\"file.url\": \"https://host/folder/esx6.5-rhel7.7-x86%5f64/esx6.5-rhel7.7-x86%5f64-flat.vmdk?dcPath=data&dsName=esx6.5-matrix\","
                                   "\"file.timeout\": 2000"
                                 "}",
                           "<source protocol='https' name='folder/esx6.5-rhel7.7-x86_64/esx6.5-rhel7.7-x86_64-flat.vmdk' query='dcPath=data&amp;dsName=esx6.5-matrix'>\n"
                           "  <host name='host' port='443'/>\n"
                           "  <ssl verify='no'/>\n"
                           "  <cookies>\n"
                           "    <cookie name='vmware_soap_session'>&quot;0c8db85112873a79b7ef74f294cb70ef7f&quot;</cookie>\n"
                           "  </cookies>\n"
                           "  <timeout seconds='2000'/>\n"
                           "</source>\n", 0);

    TEST_BACKING_PARSE_FULL("json:{ \"file.cookie\": \"vmware_soap_session=\\\"0c8db85112873a79b7ef74f294cb70ef7f\\\"\","
                                   "\"file.sslverify\": \"off\","
                                   "\"file.driver\": \"https\","
                                   "\"file.url\": \"https://host/folder/esx6.5-rhel7.7-x86%5f64/esx6.5-rhel7.7-x86%5f64-flat.vmdk?dcPath=data&dsName=esx6.5-matrix\","
                                   "\"file.timeout\": 2000"
                                 "}",
                           "<source protocol='https' name='folder/esx6.5-rhel7.7-x86_64/esx6.5-rhel7.7-x86_64-flat.vmdk' query='dcPath=data&amp;dsName=esx6.5-matrix'>\n"
                           "  <host name='host' port='443'/>\n"
                           "  <ssl verify='no'/>\n"
                           "  <cookies>\n"
                           "    <cookie name='vmware_soap_session'>&quot;0c8db85112873a79b7ef74f294cb70ef7f&quot;</cookie>\n"
                           "  </cookies>\n"
                           "  <timeout seconds='2000'/>\n"
                           "</source>\n", 0);

    TEST_BACKING_PARSE("json:{\"file\":{\"driver\": \"nvme\","
                                       "\"device\": \"0000:01:00.0\","
                                       "\"namespace\": 1"
                                      "}"
                            "}",
                        "<source type='pci' namespace='1'>\n"
                        "  <address domain='0x0000' bus='0x01' slot='0x00' function='0x0'/>\n"
                        "</source>\n");

#endif /* WITH_YAJL */

 cleanup:
    /* Final cleanup */
    testCleanupImages();

    return ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}