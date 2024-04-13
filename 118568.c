PngImg::~PngImg() {
    if(data_) delete [] data_;
}