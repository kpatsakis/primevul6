void PngImg::CopyPxlByPxl_(const PngImg& img, png_uint_32 offsetX, png_uint_32 offsetY)
{
    for(size_t x = 0; x < img.info_.width; ++x) {
        for(size_t y = 0; y < img.info_.height; ++y) {
            Set_(offsetX + x, offsetY + y, *img.Get(x, y));
        }
    }
}