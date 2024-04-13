void PngImg::Insert(const PngImg& img, png_uint_32 offsetX, png_uint_32 offsetY)
{
    if(info_.pxlsize == img.info_.pxlsize) {
        CopyRows_(img.rowPtrs_, img.info_.height, img.info_.rowbytes, offsetX, offsetY);
    } else {
        CopyPxlByPxl_(img, offsetX, offsetY);
    }
}