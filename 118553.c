void PngImg::RotateLeft() {
    Rotate_([](const Point& p, const ImgInfo& img) {
        return Point{p.y, img.width - p.x - 1};
    });
}