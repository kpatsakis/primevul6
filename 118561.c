void PngImg::Rotate_(function<Point(const Point&, const ImgInfo&)> moveFn) {
    const ImgInfo oldInfo = info_;
    const unique_ptr<png_byte[]> oldData{data_};
    const vector<png_bytep> oldRowPtrs{rowPtrs_};

    info_.width = oldInfo.height;
    info_.height = oldInfo.width;
    info_.rowbytes = info_.pxlsize * info_.width;

    InitStorage_();

    for(size_t x = 0; x < oldInfo.width; ++x) {
        for(size_t y = 0; y < oldInfo.height; ++y) {
            auto newPoint = moveFn({x, y}, oldInfo);

            png_bytep p = oldRowPtrs[y] + oldInfo.pxlsize * x;
            png_bytep newP = rowPtrs_[newPoint.y] + info_.pxlsize * newPoint.x;

            copy(p, p + info_.pxlsize, newP);
        }
    }
}