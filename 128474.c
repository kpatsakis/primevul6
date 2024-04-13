SetClientAuthSchemes(rfbClient* client,const uint32_t *authSchemes, int size)
{
  int i;

  if (client->clientAuthSchemes)
  {
    free(client->clientAuthSchemes);
    client->clientAuthSchemes = NULL;
  }
  if (authSchemes)
  {
    if (size<0)
    {
      /* If size<0 we assume the passed-in list is also 0-terminate, so we
       * calculate the size here */
      for (size=0;authSchemes[size];size++) ;
    }
    client->clientAuthSchemes = (uint32_t*)malloc(sizeof(uint32_t)*(size+1));
    for (i=0;i<size;i++)
      client->clientAuthSchemes[i] = authSchemes[i];
    client->clientAuthSchemes[size] = 0;
  }
}