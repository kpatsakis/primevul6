testPrepImages(void)
{
    int ret = EXIT_FAILURE;
    bool compat = false;
    g_autoptr(virCommand) cmd = NULL;
    g_autofree char *buf = NULL;

    qemuimg = virFindFileInPath("qemu-img");
    if (!qemuimg)
        goto skip;

    /* Clean up from any earlier failed tests */
    virFileDeleteTree(datadir);

    /* See if qemu-img supports '-o compat=xxx'.  If so, we force the
     * use of both v2 and v3 files; if not, it is v2 only but the test
     * still works. */
    cmd = virCommandNewArgList(qemuimg, "create", "-f", "qcow2",
                               "-o?", "/dev/null", NULL);
    virCommandSetOutputBuffer(cmd, &buf);
    if (virCommandRun(cmd, NULL) < 0)
        goto skip;
    if (strstr(buf, "compat "))
        compat = true;
    VIR_FREE(buf);

    absraw = g_strdup_printf("%s/raw", datadir);
    absqcow2 = g_strdup_printf("%s/qcow2", datadir);
    abswrap = g_strdup_printf("%s/wrap", datadir);
    absqed = g_strdup_printf("%s/qed", datadir);
    absdir = g_strdup_printf("%s/dir", datadir);
    abslink2 = g_strdup_printf("%s/sub/link2", datadir);

    if (virFileMakePath(datadir "/sub") < 0) {
        fprintf(stderr, "unable to create directory %s\n", datadir "/sub");
        goto cleanup;
    }
    if (virFileMakePath(datadir "/dir") < 0) {
        fprintf(stderr, "unable to create directory %s\n", datadir "/dir");
        goto cleanup;
    }

    if (chdir(datadir) < 0) {
        fprintf(stderr, "unable to test relative backing chains\n");
        goto cleanup;
    }

    buf = g_strdup_printf("%1024d", 0);
    if (virFileWriteStr("raw", buf, 0600) < 0) {
        fprintf(stderr, "unable to create raw file\n");
        goto cleanup;
    }

    /* Create a qcow2 wrapping relative raw; later on, we modify its
     * metadata to test other configurations */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "create", "-f", "qcow2", NULL);
    virCommandAddArgFormat(cmd, "-obacking_file=raw,backing_fmt=raw%s",
                           compat ? ",compat=0.10" : "");
    virCommandAddArg(cmd, "qcow2");
    if (virCommandRun(cmd, NULL) < 0)
        goto skip;
    /* Make sure our later uses of 'qemu-img rebase' will work */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "rebase", "-u", "-f", "qcow2",
                               "-F", "raw", "-b", "raw", "qcow2", NULL);
    if (virCommandRun(cmd, NULL) < 0)
        goto skip;

    /* Create a second qcow2 wrapping the first, to be sure that we
     * can correctly avoid insecure probing.  */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "create", "-f", "qcow2", NULL);
    virCommandAddArgFormat(cmd, "-obacking_file=%s,backing_fmt=qcow2%s",
                           absqcow2, compat ? ",compat=1.1" : "");
    virCommandAddArg(cmd, "wrap");
    if (virCommandRun(cmd, NULL) < 0)
        goto skip;

    /* Create a qed file. */
    virCommandFree(cmd);
    cmd = virCommandNewArgList(qemuimg, "create", "-f", "qed", NULL);
    virCommandAddArgFormat(cmd, "-obacking_file=%s,backing_fmt=raw",
                           absraw);
    virCommandAddArg(cmd, "qed");
    if (virCommandRun(cmd, NULL) < 0)
        goto skip;

#ifdef HAVE_SYMLINK
    /* Create some symlinks in a sub-directory. */
    if (symlink("../qcow2", datadir "/sub/link1") < 0 ||
        symlink("../wrap", datadir "/sub/link2") < 0) {
        fprintf(stderr, "unable to create symlink");
        goto cleanup;
    }
#endif

    ret = 0;
 cleanup:
    if (ret)
        testCleanupImages();
    return ret;

 skip:
    fputs("qemu-img is too old; skipping this test\n", stderr);
    ret = EXIT_AM_SKIP;
    goto cleanup;
}