int DISOpticalFlowImpl::autoSelectCoarsestScale(int img_width)
{
    const int fratio = 5;
    return std::max(0, (int)std::floor(log2((2.0f*(float)img_width) / ((float)fratio * (float)patch_size))));
}