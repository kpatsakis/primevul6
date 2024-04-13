void DISOpticalFlowImpl::autoSelectPatchSizeAndScales(int img_width)
{
    switch (finest_scale)
    {
    case 1:
        patch_size = 8;
        coarsest_scale = autoSelectCoarsestScale(img_width);
        finest_scale = std::max(coarsest_scale-2, 0);
        break;

    case 3:
        patch_size = 12;
        coarsest_scale = autoSelectCoarsestScale(img_width);
        finest_scale = std::max(coarsest_scale-4, 0);
        break;

    case 4:
        patch_size = 12;
        coarsest_scale = autoSelectCoarsestScale(img_width);
        finest_scale = std::max(coarsest_scale-5, 0);
        break;

    // default case, fall-through.
    case 2:
    default:
        patch_size = 8;
        coarsest_scale = autoSelectCoarsestScale(img_width);
        finest_scale = std::max(coarsest_scale-2, 0);
        break;
    }
}