  Status operator()(OpKernelContext* c, const int rows, const int cols,
                    TTypes<int>::UnalignedVec coo_row_ind,
                    TTypes<int>::UnalignedVec csr_row_ptr) {
    GpuSparse cuda_sparse(c);
    TF_RETURN_IF_ERROR(cuda_sparse.Initialize());
    return cuda_sparse.Coo2csr(coo_row_ind.data(),
                               /*nnz*/ coo_row_ind.size(),
                               /*m == rows of A*/ rows, csr_row_ptr.data());
  }