/*!***************************************************************************
*!
*! FILE NAME  : http.hh
*!
*! DESCRIPTION: HTTP, Hyper text transfer protocol.
*!
*!***************************************************************************/

#ifndef http_hh
#define http_hh

#include "job.hh"

class HTTPServer : public Job{

public:

  HTTPServer(TCPSocket* theSocket);

  void doit();

  char* extractString(char* thePosition, udword theLength);
  udword contentLength(char* theData, udword theLength);
  char* decodeBase64(char* theEncodedString);
  char* decodeForm(char* theEncodedForm,udword extraLen);
  char* findPathName(char* str,char* nameBuf);
private:
  TCPSocket* mySocket;
};

#endif
/************** END OF FILE http.cc *************************************/
