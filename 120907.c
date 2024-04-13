BOOL rectangles_intersection(const RECTANGLE_16* r1, const RECTANGLE_16* r2,
                             RECTANGLE_16* dst)
{
	dst->left = MAX(r1->left, r2->left);
	dst->right = MIN(r1->right, r2->right);
	dst->top = MAX(r1->top, r2->top);
	dst->bottom = MIN(r1->bottom, r2->bottom);
	return (dst->left < dst->right) && (dst->top < dst->bottom);
}