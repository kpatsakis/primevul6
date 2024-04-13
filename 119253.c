inline float computeSSD(uchar *I0_ptr, uchar *I1_ptr, int I0_stride, int I1_stride, float w00, float w01, float w10,
                        float w11, int patch_sz)
{
    float SSD = 0.0f;
#if CV_SIMD128
    if (patch_sz == 8)
    {
        v_float32x4 SSD_vec = v_setall_f32(0);
        HAL_INIT_BILINEAR_8x8_PATCH_EXTRACTION;
        for (int row = 0; row < 8; row++)
        {
            HAL_PROCESS_BILINEAR_8x8_PATCH_EXTRACTION;
            SSD_vec += I_diff_left * I_diff_left + I_diff_right * I_diff_right;
            HAL_BILINEAR_8x8_PATCH_EXTRACTION_NEXT_ROW;
        }
        SSD = v_reduce_sum(SSD_vec);
    }
    else
    {
#endif
        float diff;
        for (int i = 0; i < patch_sz; i++)
            for (int j = 0; j < patch_sz; j++)
            {
                diff = w00 * I1_ptr[i * I1_stride + j] + w01 * I1_ptr[i * I1_stride + j + 1] +
                       w10 * I1_ptr[(i + 1) * I1_stride + j] + w11 * I1_ptr[(i + 1) * I1_stride + j + 1] -
                       I0_ptr[i * I0_stride + j];
                SSD += diff * diff;
            }
#if CV_SIMD128
    }
#endif
    return SSD;
}