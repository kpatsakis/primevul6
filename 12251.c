testCleanupImages(void)
{
    VIR_FREE(qemuimg);
    VIR_FREE(absraw);
    VIR_FREE(absqcow2);
    VIR_FREE(abswrap);
    VIR_FREE(absqed);
    VIR_FREE(absdir);
    VIR_FREE(abslink2);

    if (chdir(abs_builddir) < 0) {
        fprintf(stderr, "unable to return to correct directory, refusing to "
                "clean up %s\n", datadir);
        return;
    }

    virFileDeleteTree(datadir);
}