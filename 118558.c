bool PngImg::InBounds_(png_uint_32 offsetX, png_uint_32 offsetY, png_uint_32 width, png_uint_32 height) const
{
    return width != 0
        && height != 0
        && width <= info_.width
        && height <= info_.height
        && offsetX < info_.width
        && offsetY < info_.height
        && offsetX + width <= info_.width
        && offsetY + height <= info_.height;
}