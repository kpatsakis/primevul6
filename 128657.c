txtwrite_close_device(gx_device * dev)
{
    int code = 0;
    gx_device_txtwrite_t *const tdev = (gx_device_txtwrite_t *) dev;

    if (tdev->file) {
        code = gx_device_close_output_file(dev, tdev->fname, tdev->file);
        tdev->file = 0;
    }

#ifdef TRACE_TXTWRITE
    fclose(tdev->DebugFile);
#endif
    return code;
}