bool PngImg::Crop(png_uint_32 offsetX, png_uint_32 offsetY, png_uint_32 width, png_uint_32 height)
{
    if(!InBounds_(offsetX, offsetY, width, height))
    {
        error_ = "Out of the bounds";
        return false;
    }

    for(size_t i = 0; i < height; ++i) {
        rowPtrs_[i] = rowPtrs_[i + offsetY] + offsetX * info_.pxlsize;
    }
    rowPtrs_.resize(height);

    info_.width = width;
    info_.height = height;
    info_.rowbytes = info_.pxlsize * width;
    return true;
}