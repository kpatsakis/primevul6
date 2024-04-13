PngImg::PngImg(const char* buf, const size_t bufLen)
    : data_(nullptr)
{
    memset(&info_, 0, sizeof(info_));
    PngReadStruct rs;
    if(rs.Valid()) {
        BufPtr bufPtr = {buf, bufLen};
        png_set_read_fn(rs.pngPtr, (png_voidp)&bufPtr, readFromBuf);
        ReadInfo_(rs);

        InitStorage_();
        png_read_image(rs.pngPtr, &rowPtrs_[0]);
    }
}