virStorageVolFDStreamCloseCb(virStreamPtr st G_GNUC_UNUSED,
                             void *opaque)
{
    virThread thread;

    if (virThreadCreateFull(&thread, false, virStorageVolPoolRefreshThread,
                            "vol-refresh", false, opaque) < 0) {
        /* Not much else can be done */
        VIR_ERROR(_("Failed to create thread to handle pool refresh"));
        goto error;
    }
    return; /* Thread will free opaque data */

 error:
    virStorageVolPoolRefreshDataFree(opaque);
}