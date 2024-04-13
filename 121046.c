BOOL rectangle_is_empty(const RECTANGLE_16* rect)
{
	/* A rectangle with width = 0 or height = 0 should be regarded
	 * as empty.
	 */
	return ((rect->left == rect->right) || (rect->top == rect->bottom)) ? TRUE : FALSE;
}