txtwrite_open_device(gx_device * dev)
{
    gx_device_txtwrite_t *const tdev = (gx_device_txtwrite_t *) dev;
    int code = 0;

    gx_device_fill_in_procs(dev);
    if (tdev->fname[0] == 0)
        return_error(gs_error_undefinedfilename);

    tdev->PageData.PageNum = 0;
    tdev->PageData.y_ordered_list = NULL;
    tdev->file = NULL;
#ifdef TRACE_TXTWRITE
    tdev->DebugFile = gp_fopen(dev->memory,"/temp/txtw_dbg.txt", "wb+");
#endif
    dev->color_info.separable_and_linear = GX_CINFO_SEP_LIN;
    set_linear_color_bits_mask_shift(dev);
    dev->interpolate_control = 0;

    code = install_internal_subclass_devices((gx_device **)&dev, NULL);
    return code;
}