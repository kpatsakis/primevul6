unique_ptr<Pxl> PngImg::Get(png_uint_32 x, png_uint_32 y) const
{
    if(x >= info_.width || y >= info_.height)
    {
        error_ = "Out of the bounds";
        return nullptr;
    }

    png_bytep p = rowPtrs_[y] + info_.pxlsize * x;
    unique_ptr<Pxl> pPxl(new Pxl{0, 0, 0, 0});
    pPxl->r = p[0];
    pPxl->g = p[1];
    pPxl->b = p[2];
    pPxl->a = info_.pxlsize > 3 ? p[3] : 255;

    return pPxl;
}