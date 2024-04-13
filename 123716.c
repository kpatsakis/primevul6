transform_3d(FT_Vector shift, ASS_Outline *outline, ASS_Outline *border,
             double frx, double fry, double frz, double fax, double fay,
             double scale, int yshift)
{
    frx = -frx;
    frz = -frz;
    if (frx != 0. || fry != 0. || frz != 0. || fax != 0. || fay != 0.) {
        if (outline)
            transform_3d_points(shift, outline, frx, fry, frz,
                                fax, fay, scale, yshift);

        if (border)
            transform_3d_points(shift, border, frx, fry, frz,
                                fax, fay, scale, yshift);
    }
}