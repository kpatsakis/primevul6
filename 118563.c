bool PngImg::Fill(png_uint_32 offsetX, png_uint_32 offsetY, png_uint_32 width, png_uint_32 height, const Pxl& pxl)
{
    if(!InBounds_(offsetX, offsetY, width, height))
    {
        error_ = "Out of the bounds";
        return false;
    }

    for(size_t i = 0; i < height; ++i) {
        for(size_t j = 0; j < width; ++j) {
            Set_(offsetX + j, offsetY + i, pxl);
        }
    }

    return true;
}