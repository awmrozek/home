/*!***************************************************************************
*!
*! FILE NAME  : http.cc
*!
*! DESCRIPTION: HTTP, Hyper text transfer protocol.
*!
*!***************************************************************************/

/****************** INCLUDE FILES SECTION ***********************************/

#include "compiler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern "C"
{
  #include "system.h"
}

#include "iostream.hh"
#include "tcpsocket.hh"
#include "http.hh"
#include "fs.hh"

//#define D_HTTP
#ifdef D_HTTP
#define trace cout
#else
#define trace if(false) cout
#endif

/****************** HTTPServer DEFINITION SECTION ***************************/
#define REC_BUFFER_SIZE 10000

//----------------------------------------------------------------------------
//
// Allocates a new null terminated string containing a copy of the data at
// 'thePosition', 'theLength' characters long. The string must be deleted by
// the caller.
//



HTTPServer::HTTPServer(TCPSocket* theSocket) : mySocket(theSocket) {}

void HTTPServer::doit() {

  char* receiveBuffer = new char[REC_BUFFER_SIZE];

  if(receiveBuffer == NULL) {
    cout << "HTTPServer: Out of memory" << endl;
    mySocket->Close();
    return;
  }

  udword nrec = 0;
  udword length;
  char* crlf = NULL;

  while(!mySocket->isEof() && crlf == NULL) {

    byte* data = mySocket->Read(length);

    if(length > 0) {
      if(nrec + length >= REC_BUFFER_SIZE) {
        nrec = 0;
        cout << "HTTPServer: Full receive buffer!" << endl;
        delete[] data;
        break;
      } else {
        memcpy(receiveBuffer + nrec,data,length);
        delete[] data;
        receiveBuffer[nrec + length] = 0;
        crlf = strstr(receiveBuffer,"\r\n\r\n");
        nrec += length;
        //  if(crlf != NULL) break;
      }
    }
  }

  if(crlf != NULL) {

    // Receive buffer contains entire HTTP request header

    if(strncmp(receiveBuffer,"GET",3) == 0 || strncmp(receiveBuffer,"HEAD",4) == 0) {
      nrec = 0;
      udword fileLength;

      char name[32];
      char* basic = strstr(receiveBuffer, "Authorization: Basic");
      char* pathName = findPathName(receiveBuffer,name);
      bool doSend = true;

      if(strstr(pathName,"private") != NULL) {
        doSend = false;

        if(basic != NULL) {
          basic += 21;
          char* end = strchr(basic,'\r');
          end[0] = 0;
          basic = decodeBase64(basic);
          doSend = strcmp(basic,"hej:123")  == 0;

          delete[] basic;
        }
        const char* unauth = "HTTP/1.0 401 Unauthorized\r\n"
        "Content-Type: text/html\r\n"
        "WWW-Authenticate: Basic realm=\"private\"\r\n\r\n"
        "<html><head><title>401 Unauthorized</title></head>\r\n"
        "<body><h1>401 Unauthorized</h1></body></html>";

        if(!doSend) mySocket->Write((const byte*)unauth,strlen(unauth));

      }

      if(doSend) {
        char reply[512];
        int index = 0;

        if(strstr(name,".gif")) index = 1;
        else if(strstr(name,".jpg")) index = 2;

        const byte* file = FileSystem::instance().readFile(pathName,name,fileLength);
        static const char* formatTable[3] = {
          "text/html",
          "image/gif",
          "image/jpeg",
        };

        if(file != NULL) {
          sprintf(reply,"HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n",formatTable[index]);
          mySocket->Write((const byte*)reply,strlen(reply));

          if(receiveBuffer[0] == 'G')
            mySocket->Write(file,fileLength);
        } else {
          sprintf(reply,"HTTP/1.0 404 Not found\r\nContent-type: %s\r\n\r\n"
          "<html><head><title>File not found</title></head>"
          "<body><h1>404 Not found</h1></body></html>",formatTable[0]);

          mySocket->Write((const byte*)reply,strlen(reply));
        }
      }

    }
    else if(strncmp(receiveBuffer,"POST",4) == 0 ) {

      udword cl = contentLength(receiveBuffer,nrec);
      char* payload = crlf + 4;

      while( !mySocket->isEof() && nrec - (udword)(payload - receiveBuffer) < cl) {
        byte* data = mySocket->Read(length);
        if(length != 0) {
          if(nrec + length > REC_BUFFER_SIZE) {
            cout << "Full buffer!" << endl;
            delete[] data;
            break;
          } else {
            memcpy(receiveBuffer + nrec,data,length);
          }
          nrec += length;
          receiveBuffer[nrec] = 0;
          delete[] data;
        }
      }

      //ugly hardcode for file to be replaced ._.
      static const char* pathName = "dynamic\xff";
      static const char* name = "dynamic.htm";


      udword extraLen = strlen(pathName)+strlen(name)+2 + sizeof(ChangedFile);
      char* text = decodeForm(crlf + 4,extraLen);

      if(text != NULL) {
        FileSystem::instance().writeFile(pathName,name,(byte*)text,strlen(text));

        const char* reply32 =
        "HTTP/1.0 200 OK\r\n"
        "Content-type: text/html\r\n"
        "\r\n"
        "<html><head><title>Accepted</title></head>"
        "<body><h1>The file dynamic.htm was updated successfully.</h1></body></html>";
        mySocket->Write((const byte*)reply32,strlen(reply32));
      } else {

        const char* reply32 =
        "HTTP/1.0 200 OK\r\n"
        "Content-type: text/html\r\n"
        "\r\n"
        "<html><head><title>Accepted</title></head>"
        "<body><h1>Out Of Memory.</h1></body></html>";
        mySocket->Write((const byte*)reply32,strlen(reply32));

      }

    }

  }

  mySocket->Close();
  delete[] receiveBuffer;
  // "Stopped HTTP server" << endl;
}

udword HTTPServer::contentLength(char* theData, udword theLength) {
  udword index = 0;
  bool   lenFound = false;
  const char* aSearchString = "Content-Length: ";
  while ((index++ < theLength) && !lenFound)
  {
    lenFound = (strncmp(theData + index,
      aSearchString,
      strlen(aSearchString)) == 0);
    }
    if (!lenFound)
    {
      return 0;
    }
    index += strlen(aSearchString) - 1;
    char* lenStart = theData + index;
    char* lenEnd = strchr(theData + index, '\r');
    lenEnd[0] = 0;
    udword contLen = atoi(lenStart);
    lenEnd[0] = '\r';
    return contLen;
  }

  //----------------------------------------------------------------------------
  //
  // Decode user and password for basic authentication.
  // returns a decoded string that must be deleted by the caller.
  //

char* HTTPServer::decodeBase64(char* theEncodedString) {
    static const char* someValidCharacters =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

    int aCharsToDecode;
    int k = 0;
    char  aTmpStorage[4];
    int aValue;
    char* aResult = new char[80];

    // Original code by JH, found on the net years later (!).
    // Modify on your own risk.

    for (unsigned int i = 0; i < strlen(theEncodedString); i += 4)
    {
      aValue = 0;
      aCharsToDecode = 3;
      if (theEncodedString[i+2] == '=')
      {
        aCharsToDecode = 1;
      }
      else if (theEncodedString[i+3] == '=')
      {
        aCharsToDecode = 2;
      }

      for (int j = 0; j <= aCharsToDecode; j++)
      {
        int aDecodedValue;
        aDecodedValue = strchr(someValidCharacters,theEncodedString[i+j])
        - someValidCharacters;
        aDecodedValue <<= ((3-j)*6);
        aValue += aDecodedValue;
      }
      for (int jj = 2; jj >= 0; jj--)
      {
        aTmpStorage[jj] = aValue & 255;
        aValue >>= 8;
      }
      aResult[k++] = aTmpStorage[0];
      aResult[k++] = aTmpStorage[1];
      aResult[k++] = aTmpStorage[2];
    }
    aResult[k] = 0; // zero terminate string

    return aResult;
}

char* HTTPServer::decodeForm(char* theEncodedForm,udword extraLen) {
    char* anEncodedFile = strchr(theEncodedForm,'=');
    anEncodedFile++;
    char* aForm = new char[strlen(anEncodedFile) * 2 + extraLen];

    if(aForm == NULL) {
      cout << "can not allocate file" << endl;
      return NULL;
    }

    //  cout << "new[] " << hex << (udword)(aForm) << dec << endl;

    aForm += extraLen;
    // Serious overkill, but what the heck, we've got plenty of memory here!
    udword aSourceIndex = 0;
    udword aDestIndex = 0;

    while (aSourceIndex < strlen(anEncodedFile))
    {
      char aChar = *(anEncodedFile + aSourceIndex++);
      switch (aChar)
      {
        case '&':
        *(aForm + aDestIndex++) = '\r';
        *(aForm + aDestIndex++) = '\n';
        break;
        case '+':
        *(aForm + aDestIndex++) = ' ';
        break;
        case '%':
        char aTemp[5];
        aTemp[0] = '0';
        aTemp[1] = 'x';
        aTemp[2] = *(anEncodedFile + aSourceIndex++);
        aTemp[3] = *(anEncodedFile + aSourceIndex++);
        aTemp[4] = '\0';
        udword anUdword;
        anUdword = strtoul((char*)&aTemp,0,0);
        *(aForm + aDestIndex++) = (char)anUdword;
        break;
        default:
        *(aForm + aDestIndex++) = aChar;
        break;
      }
    }
    *(aForm + aDestIndex++) = '\0';
    return aForm;
}

char* HTTPServer::findPathName(char* str,char* nameBuf) {

    strcpy(nameBuf,"index.htm");

    char* firstPos = strchr(str, ' ');     // First space on line
    firstPos++;                            // Pointer to first /
    char* lastPos = strchr(firstPos, ' '); // Last space on line
    char* thePath = 0;                     // Result path
    if ((lastPos - firstPos) == 1) {
      // "only /" << endl;
      // Is / only
      return "";

    } else {

      *lastPos = 0;

      // Is an absolute path. Skip first /.
      thePath = firstPos+1;
      if ((lastPos = strrchr(thePath, '/')) != 0)
      {
        // Found a path. Insert -1 as terminator.

        strcpy(nameBuf,lastPos + 1);

        lastPos[1] = 0;

        while((lastPos = strchr(thePath, '/')) != NULL) lastPos[0] = '\xff';

        return thePath;

      } else {

        // Is /index.html
        return ""; // Return NULL
      }

    }
}

  /************** END OF FILE http.cc *************************************/
