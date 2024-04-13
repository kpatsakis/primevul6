HandleVncAuth(rfbClient *client)
{
    uint8_t challenge[CHALLENGESIZE];
    char *passwd=NULL;
    int i;

    if (!ReadFromRFBServer(client, (char *)challenge, CHALLENGESIZE)) return FALSE;

    if (client->serverPort!=-1) { /* if not playing a vncrec file */
      if (client->GetPassword)
        passwd = client->GetPassword(client);

      if ((!passwd) || (strlen(passwd) == 0)) {
        rfbClientLog("Reading password failed\n");
        return FALSE;
      }
      if (strlen(passwd) > 8) {
        passwd[8] = '\0';
      }

      rfbClientEncryptBytes(challenge, passwd);

      /* Lose the password from memory */
      for (i = strlen(passwd); i >= 0; i--) {
        passwd[i] = '\0';
      }
      free(passwd);

      if (!WriteToRFBServer(client, (char *)challenge, CHALLENGESIZE)) return FALSE;
    }

    /* Handle the SecurityResult message */
    if (!rfbHandleAuthResult(client)) return FALSE;

    return TRUE;
}