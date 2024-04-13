transform_3d_points(FT_Vector shift, ASS_Outline *outline, double frx, double fry,
                    double frz, double fax, double fay, double scale,
                    int yshift)
{
    double sx = sin(frx);
    double sy = sin(fry);
    double sz = sin(frz);
    double cx = cos(frx);
    double cy = cos(fry);
    double cz = cos(frz);
    FT_Vector *p = outline->points;
    double x, y, z, xx, yy, zz;
    int dist;

    dist = 20000 * scale;
    for (size_t i = 0; i < outline->n_points; ++i) {
        x = (double) p[i].x + shift.x + (fax * (yshift - p[i].y));
        y = (double) p[i].y + shift.y + (-fay * p[i].x);
        z = 0.;

        xx = x * cz + y * sz;
        yy = -(x * sz - y * cz);
        zz = z;

        x = xx;
        y = yy * cx + zz * sx;
        z = yy * sx - zz * cx;

        xx = x * cy + z * sy;
        yy = y;
        zz = x * sy - z * cy;

        zz = FFMAX(zz, 1000 - dist);

        x = (xx * dist) / (zz + dist);
        y = (yy * dist) / (zz + dist);
        p[i].x = x - shift.x + 0.5;
        p[i].y = y - shift.y + 0.5;
    }
}