void PngImg::RotateRight() {
    Rotate_([](const Point& p, const ImgInfo& img) {
        return Point{img.height - p.y - 1, p.x};
    });
}