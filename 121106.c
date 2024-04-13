BOOL rectangles_equal(const RECTANGLE_16* r1, const RECTANGLE_16* r2)
{
	return ((r1->left == r2->left) && (r1->top == r2->top) &&
	        (r1->right == r2->right) && (r1->bottom == r2->bottom)) ? TRUE : FALSE;
}