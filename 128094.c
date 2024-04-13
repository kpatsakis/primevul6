}

/* put_params is supposed to check all the parameters before setting any. */
static int
lxm_put_params(gx_device *pdev, gs_param_list *plist)
{
    int ecode;
    lxm_device* const ldev = (lxm_device*)pdev;
    int trialHeadSeparation=ldev->headSeparation;
    int code = param_read_int(plist, "HeadSeparation", &trialHeadSeparation);

    if ( trialHeadSeparation < 1 || trialHeadSeparation > 32 )
        param_signal_error(plist, "HeadSeparation", gs_error_rangecheck);
    /* looks like param_signal_error is not expected to return */
    ecode = gdev_prn_put_params(pdev, plist);	/* call super class put_params */
    if ( code < 0 ) return code;
    if (ecode < 0) return ecode;

    /* looks like everything okay; go ahead and set headSeparation */