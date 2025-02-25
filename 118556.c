void PngImg::CopyRows_(const vector<png_bytep>& rowPtrs, const size_t numRows, const size_t rowLen,
    png_uint_32 offsetX, png_uint_32 offsetY)
{
    for(size_t y = 0; y < numRows; ++y) {
        memcpy(rowPtrs_[y + offsetY] + offsetX * info_.pxlsize, rowPtrs[y], rowLen);
    }
}