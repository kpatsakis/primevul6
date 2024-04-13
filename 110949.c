static void vp9_compute_image_size(int FrameWidth, int FrameHeight, int *Sb64Cols, int *Sb64Rows)
{
	int MiCols = (FrameWidth + 7) >> 3;
	int MiRows = (FrameHeight + 7) >> 3;
	*Sb64Cols = (MiCols + 7) >> 3;
	*Sb64Rows = (MiRows + 7) >> 3;
}