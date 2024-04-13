void BZ2File::sweep() {
  closeImpl();
  File::sweep();
}