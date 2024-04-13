FreeUserCredential(rfbCredential *cred)
{
  if (cred->userCredential.username) free(cred->userCredential.username);
  if (cred->userCredential.password) free(cred->userCredential.password);
  free(cred);
}