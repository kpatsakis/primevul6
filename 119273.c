void DISOpticalFlowImpl::ocl_prepareBuffers(UMat &I0, UMat &I1, UMat &flow, bool use_flow)
{
    u_I0s.resize(coarsest_scale + 1);
    u_I1s.resize(coarsest_scale + 1);
    u_I1s_ext.resize(coarsest_scale + 1);
    u_I0xs.resize(coarsest_scale + 1);
    u_I0ys.resize(coarsest_scale + 1);
    u_Ux.resize(coarsest_scale + 1);
    u_Uy.resize(coarsest_scale + 1);

    vector<UMat> flow_uv(2);
    if (use_flow)
    {
        split(flow, flow_uv);
        u_initial_Ux.resize(coarsest_scale + 1);
        u_initial_Uy.resize(coarsest_scale + 1);
    }

    int fraction = 1;
    int cur_rows = 0, cur_cols = 0;

    for (int i = 0; i <= coarsest_scale; i++)
    {
        /* Avoid initializing the pyramid levels above the finest scale, as they won't be used anyway */
        if (i == finest_scale)
        {
            cur_rows = I0.rows / fraction;
            cur_cols = I0.cols / fraction;
            u_I0s[i].create(cur_rows, cur_cols, CV_8UC1);
            resize(I0, u_I0s[i], u_I0s[i].size(), 0.0, 0.0, INTER_AREA);
            u_I1s[i].create(cur_rows, cur_cols, CV_8UC1);
            resize(I1, u_I1s[i], u_I1s[i].size(), 0.0, 0.0, INTER_AREA);

            /* These buffers are reused in each scale so we initialize them once on the finest scale: */
            u_Sx.create(cur_rows / patch_stride, cur_cols / patch_stride, CV_32FC1);
            u_Sy.create(cur_rows / patch_stride, cur_cols / patch_stride, CV_32FC1);
            u_I0xx_buf.create(cur_rows / patch_stride, cur_cols / patch_stride, CV_32FC1);
            u_I0yy_buf.create(cur_rows / patch_stride, cur_cols / patch_stride, CV_32FC1);
            u_I0xy_buf.create(cur_rows / patch_stride, cur_cols / patch_stride, CV_32FC1);
            u_I0x_buf.create(cur_rows / patch_stride, cur_cols / patch_stride, CV_32FC1);
            u_I0y_buf.create(cur_rows / patch_stride, cur_cols / patch_stride, CV_32FC1);

            u_I0xx_buf_aux.create(cur_rows, cur_cols / patch_stride, CV_32FC1);
            u_I0yy_buf_aux.create(cur_rows, cur_cols / patch_stride, CV_32FC1);
            u_I0xy_buf_aux.create(cur_rows, cur_cols / patch_stride, CV_32FC1);
            u_I0x_buf_aux.create(cur_rows, cur_cols / patch_stride, CV_32FC1);
            u_I0y_buf_aux.create(cur_rows, cur_cols / patch_stride, CV_32FC1);

            u_U.create(cur_rows, cur_cols, CV_32FC2);
        }
        else if (i > finest_scale)
        {
            cur_rows = u_I0s[i - 1].rows / 2;
            cur_cols = u_I0s[i - 1].cols / 2;
            u_I0s[i].create(cur_rows, cur_cols, CV_8UC1);
            resize(u_I0s[i - 1], u_I0s[i], u_I0s[i].size(), 0.0, 0.0, INTER_AREA);
            u_I1s[i].create(cur_rows, cur_cols, CV_8UC1);
            resize(u_I1s[i - 1], u_I1s[i], u_I1s[i].size(), 0.0, 0.0, INTER_AREA);
        }

        if (i >= finest_scale)
        {
            u_I1s_ext[i].create(cur_rows + 2 * border_size, cur_cols + 2 * border_size, CV_8UC1);
            copyMakeBorder(u_I1s[i], u_I1s_ext[i], border_size, border_size, border_size, border_size, BORDER_REPLICATE);
            u_I0xs[i].create(cur_rows, cur_cols, CV_16SC1);
            u_I0ys[i].create(cur_rows, cur_cols, CV_16SC1);
            spatialGradient(u_I0s[i], u_I0xs[i], u_I0ys[i]);
            u_Ux[i].create(cur_rows, cur_cols, CV_32FC1);
            u_Uy[i].create(cur_rows, cur_cols, CV_32FC1);
            variational_refinement_processors[i]->setAlpha(variational_refinement_alpha);
            variational_refinement_processors[i]->setDelta(variational_refinement_delta);
            variational_refinement_processors[i]->setGamma(variational_refinement_gamma);
            variational_refinement_processors[i]->setSorIterations(5);
            variational_refinement_processors[i]->setFixedPointIterations(variational_refinement_iter);

            if (use_flow)
            {
                resize(flow_uv[0], u_initial_Ux[i], Size(cur_cols, cur_rows));
                divide(u_initial_Ux[i], static_cast<float>(fraction), u_initial_Ux[i]);
                resize(flow_uv[1], u_initial_Uy[i], Size(cur_cols, cur_rows));
                divide(u_initial_Uy[i], static_cast<float>(fraction), u_initial_Uy[i]);
            }
        }

        fraction *= 2;
    }
}