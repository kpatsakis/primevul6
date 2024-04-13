void DISOpticalFlowImpl::precomputeStructureTensor(Mat &dst_I0xx, Mat &dst_I0yy, Mat &dst_I0xy, Mat &dst_I0x,
                                                   Mat &dst_I0y, Mat &I0x, Mat &I0y)
{
    float *I0xx_ptr = dst_I0xx.ptr<float>();
    float *I0yy_ptr = dst_I0yy.ptr<float>();
    float *I0xy_ptr = dst_I0xy.ptr<float>();
    float *I0x_ptr = dst_I0x.ptr<float>();
    float *I0y_ptr = dst_I0y.ptr<float>();

    float *I0xx_aux_ptr = I0xx_buf_aux.ptr<float>();
    float *I0yy_aux_ptr = I0yy_buf_aux.ptr<float>();
    float *I0xy_aux_ptr = I0xy_buf_aux.ptr<float>();
    float *I0x_aux_ptr = I0x_buf_aux.ptr<float>();
    float *I0y_aux_ptr = I0y_buf_aux.ptr<float>();

    /* Separable box filter: horizontal pass */
    for (int i = 0; i < h; i++)
    {
        float sum_xx = 0.0f, sum_yy = 0.0f, sum_xy = 0.0f, sum_x = 0.0f, sum_y = 0.0f;
        short *x_row = I0x.ptr<short>(i);
        short *y_row = I0y.ptr<short>(i);
        for (int j = 0; j < patch_size; j++)
        {
            sum_xx += x_row[j] * x_row[j];
            sum_yy += y_row[j] * y_row[j];
            sum_xy += x_row[j] * y_row[j];
            sum_x += x_row[j];
            sum_y += y_row[j];
        }
        I0xx_aux_ptr[i * ws] = sum_xx;
        I0yy_aux_ptr[i * ws] = sum_yy;
        I0xy_aux_ptr[i * ws] = sum_xy;
        I0x_aux_ptr[i * ws] = sum_x;
        I0y_aux_ptr[i * ws] = sum_y;
        int js = 1;
        for (int j = patch_size; j < w; j++)
        {
            sum_xx += (x_row[j] * x_row[j] - x_row[j - patch_size] * x_row[j - patch_size]);
            sum_yy += (y_row[j] * y_row[j] - y_row[j - patch_size] * y_row[j - patch_size]);
            sum_xy += (x_row[j] * y_row[j] - x_row[j - patch_size] * y_row[j - patch_size]);
            sum_x += (x_row[j] - x_row[j - patch_size]);
            sum_y += (y_row[j] - y_row[j - patch_size]);
            if ((j - patch_size + 1) % patch_stride == 0)
            {
                I0xx_aux_ptr[i * ws + js] = sum_xx;
                I0yy_aux_ptr[i * ws + js] = sum_yy;
                I0xy_aux_ptr[i * ws + js] = sum_xy;
                I0x_aux_ptr[i * ws + js] = sum_x;
                I0y_aux_ptr[i * ws + js] = sum_y;
                js++;
            }
        }
    }

    AutoBuffer<float> sum_xx(ws), sum_yy(ws), sum_xy(ws), sum_x(ws), sum_y(ws);
    for (int j = 0; j < ws; j++)
    {
        sum_xx[j] = 0.0f;
        sum_yy[j] = 0.0f;
        sum_xy[j] = 0.0f;
        sum_x[j] = 0.0f;
        sum_y[j] = 0.0f;
    }

    /* Separable box filter: vertical pass */
    for (int i = 0; i < patch_size; i++)
        for (int j = 0; j < ws; j++)
        {
            sum_xx[j] += I0xx_aux_ptr[i * ws + j];
            sum_yy[j] += I0yy_aux_ptr[i * ws + j];
            sum_xy[j] += I0xy_aux_ptr[i * ws + j];
            sum_x[j] += I0x_aux_ptr[i * ws + j];
            sum_y[j] += I0y_aux_ptr[i * ws + j];
        }
    for (int j = 0; j < ws; j++)
    {
        I0xx_ptr[j] = sum_xx[j];
        I0yy_ptr[j] = sum_yy[j];
        I0xy_ptr[j] = sum_xy[j];
        I0x_ptr[j] = sum_x[j];
        I0y_ptr[j] = sum_y[j];
    }
    int is = 1;
    for (int i = patch_size; i < h; i++)
    {
        for (int j = 0; j < ws; j++)
        {
            sum_xx[j] += (I0xx_aux_ptr[i * ws + j] - I0xx_aux_ptr[(i - patch_size) * ws + j]);
            sum_yy[j] += (I0yy_aux_ptr[i * ws + j] - I0yy_aux_ptr[(i - patch_size) * ws + j]);
            sum_xy[j] += (I0xy_aux_ptr[i * ws + j] - I0xy_aux_ptr[(i - patch_size) * ws + j]);
            sum_x[j] += (I0x_aux_ptr[i * ws + j] - I0x_aux_ptr[(i - patch_size) * ws + j]);
            sum_y[j] += (I0y_aux_ptr[i * ws + j] - I0y_aux_ptr[(i - patch_size) * ws + j]);
        }
        if ((i - patch_size + 1) % patch_stride == 0)
        {
            for (int j = 0; j < ws; j++)
            {
                I0xx_ptr[is * ws + j] = sum_xx[j];
                I0yy_ptr[is * ws + j] = sum_yy[j];
                I0xy_ptr[is * ws + j] = sum_xy[j];
                I0x_ptr[is * ws + j] = sum_x[j];
                I0y_ptr[is * ws + j] = sum_y[j];
            }
            is++;
        }
    }
}