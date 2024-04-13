inline float processPatch(float &dst_dUx, float &dst_dUy, uchar *I0_ptr, uchar *I1_ptr, short *I0x_ptr, short *I0y_ptr,
                          int I0_stride, int I1_stride, float w00, float w01, float w10, float w11, int patch_sz)
{
    float SSD = 0.0f;
#if CV_SIMD128
    if (patch_sz == 8)
    {
        /* Variables to accumulate the sums */
        v_float32x4 Ux_vec = v_setall_f32(0);
        v_float32x4 Uy_vec = v_setall_f32(0);
        v_float32x4 SSD_vec = v_setall_f32(0);

        v_int16x8 I0x_row, I0y_row;
        v_int32x4 I0x_row_4_left, I0x_row_4_right, I0y_row_4_left, I0y_row_4_right;

        HAL_INIT_BILINEAR_8x8_PATCH_EXTRACTION;
        for (int row = 0; row < 8; row++)
        {
            HAL_PROCESS_BILINEAR_8x8_PATCH_EXTRACTION;
            I0x_row = v_load(I0x_ptr);
            v_expand(I0x_row, I0x_row_4_left, I0x_row_4_right);
            I0y_row = v_load(I0y_ptr);
            v_expand(I0y_row, I0y_row_4_left, I0y_row_4_right);

            /* Update the sums: */
            Ux_vec += I_diff_left * v_cvt_f32(I0x_row_4_left) + I_diff_right * v_cvt_f32(I0x_row_4_right);
            Uy_vec += I_diff_left * v_cvt_f32(I0y_row_4_left) + I_diff_right * v_cvt_f32(I0y_row_4_right);
            SSD_vec += I_diff_left * I_diff_left + I_diff_right * I_diff_right;

            I0x_ptr += I0_stride;
            I0y_ptr += I0_stride;
            HAL_BILINEAR_8x8_PATCH_EXTRACTION_NEXT_ROW;
        }

        /* Final reduce operations: */
        dst_dUx = v_reduce_sum(Ux_vec);
        dst_dUy = v_reduce_sum(Uy_vec);
        SSD = v_reduce_sum(SSD_vec);
    }
    else
    {
#endif
        dst_dUx = 0.0f;
        dst_dUy = 0.0f;
        float diff;
        for (int i = 0; i < patch_sz; i++)
            for (int j = 0; j < patch_sz; j++)
            {
                diff = w00 * I1_ptr[i * I1_stride + j] + w01 * I1_ptr[i * I1_stride + j + 1] +
                       w10 * I1_ptr[(i + 1) * I1_stride + j] + w11 * I1_ptr[(i + 1) * I1_stride + j + 1] -
                       I0_ptr[i * I0_stride + j];

                SSD += diff * diff;
                dst_dUx += diff * I0x_ptr[i * I0_stride + j];
                dst_dUy += diff * I0y_ptr[i * I0_stride + j];
            }
#if CV_SIMD128
    }
#endif
    return SSD;
}