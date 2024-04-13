void PngImg::SetSize(png_uint_32 width, png_uint_32 height)
{
    const ImgInfo oldInfo = info_;
    const unique_ptr<png_byte[]> oldData{data_};
    const vector<png_bytep> oldRowPtrs{rowPtrs_};

    info_.width = width;
    info_.height = height;
    info_.rowbytes = info_.pxlsize * width;

    InitStorage_();
    memset(data_, 0, info_.height * info_.rowbytes);
    CopyRows_(oldRowPtrs, min(height, oldInfo.height), min(oldInfo.rowbytes, info_.rowbytes));
}