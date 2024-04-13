__global__ void TridiagonalMatMulKernel(int batch_size, int m, int n,
                                        const Scalar* __restrict__ superdiag,
                                        const Scalar* __restrict__ maindiag,
                                        const Scalar* __restrict__ subdiag,
                                        const Scalar* __restrict__ rhs,
                                        Scalar* __restrict__ product) {
  for (int i : CudaGridRangeX(batch_size * m * n)) {
    int row_id = i / n;
    Scalar result = maindiag[row_id] * rhs[i];
    if (row_id % m != 0) {
      result = result + subdiag[row_id] * rhs[i - n];
    }
    if ((row_id + 1) % m != 0) {
      result = result + superdiag[row_id] * rhs[i + n];
    }
    product[i] = result;
  }
}