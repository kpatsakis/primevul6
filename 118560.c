void PngImg::Set_(png_uint_32 x, png_uint_32 y, const Pxl& pxl)
{
    png_bytep p = rowPtrs_[y] + info_.pxlsize * x;
    p[0] = pxl.r;
    p[1] = pxl.g;
    p[2] = pxl.b;
    if(info_.pxlsize > 3) {
        p[3] = pxl.a;
    }
}