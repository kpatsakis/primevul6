static AffineMatrix InverseAffineMatrix(const AffineMatrix *affine)
{
  AffineMatrix
    inverse_affine;

  double
    determinant;

  determinant=PerceptibleReciprocal(affine->sx*affine->sy-affine->rx*
    affine->ry);
  inverse_affine.sx=determinant*affine->sy;
  inverse_affine.rx=determinant*(-affine->rx);
  inverse_affine.ry=determinant*(-affine->ry);
  inverse_affine.sy=determinant*affine->sx;
  inverse_affine.tx=(-affine->tx)*inverse_affine.sx-affine->ty*
    inverse_affine.ry;
  inverse_affine.ty=(-affine->tx)*inverse_affine.rx-affine->ty*
    inverse_affine.sy;
  return(inverse_affine);
}