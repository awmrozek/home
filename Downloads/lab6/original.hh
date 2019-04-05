/*!***************************************************************************
*!
*! FILE NAME  : tcpsocket.hh
*! 
*! DESCRIPTION: TCPSocket is the TCP protocols stacks interface to the
*!              application.
*! 
*!***************************************************************************/

#ifndef tcpsocket_hh
#define tcpsocket_hh

/****************** INCLUDE FILES SECTION ***********************************/

#include "job.hh"

/****************** CLASS DEFINITION SECTION ********************************/

/*****************************************************************************
*%
*% CLASS NAME   : TCPSocket
*%
*% BASE CLASSES : None
*%
*% CLASS TYPE   : Singleton
*%
*% DESCRIPTION  : Handles the interace beetwen TCP and the application.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class TCPConnection;
class Semaphore;
class TCPSocket
{
 public:
  TCPSocket(TCPConnection* theConnection);
  // Constructor. The socket is created by class TCP when a connection is
  // established. create the semaphores
  ~TCPSocket();
  // Destructor. destroy the semaphores.

  //
  // Interface to application
  //

  byte* Read(udword& theLength);
  // Read incoming data segment. This call will block on the read semaphore
  // until data is available.
  // theLength returns the amount of data read.
  bool  isEof();
  // True if a FIN has been received from the remote host.
  void  Write(byte* theData, udword theLength);
  // Write data to remote host. This call will block on the write semaphore
  // until all the data has been sent to, and acknowledged by, the remote host.
  void  Close();
  // Close the socket. When called the socket will close the connection by
  // calling myConnection->AppClose() and delete itself.

  //
  // Interface to TCPConnection
  //
  
  void socketDataReceived(byte* theData, udword theLength);
  // Called from TCP when data has been received. signals the read semaphore.
  void socketDataSent();
  // Called from TCP when all data has been sent and acknowledged. signals
  // the write semaphore.
  void socketEof();
  // Called from TCP when a FIN has been received in the established state.
  // Sets the eofFound flag and signals the read semaphore.

 private:
  TCPConnection*  myConnection;
  // Pointer to the connection asociated with this socket.
  Semaphore*      myReadSemaphore;
  // Blocks the read operation until data is received or the connection closed.
  Semaphore*      myWriteSemaphore;
  // Blocks the write operation until all data has been sent.

  byte*           myReadData;
  udword          myReadLength;
  // Holds received data beetwen the time that socketDataReceived is called
  // and the Read operation wakes up and passses the data to the application.
  
  bool  eofFound;
  // Connection has been closed by the remote host
};

/*****************************************************************************
*%
*% CLASS NAME   : SimpleApplication
*%
*% BASE CLASSES : Job
*%
*% CLASS TYPE   : 
*%
*% DESCRIPTION  : Example application
*%
*% SUBCLASSING  : None
*%
*%***************************************************************************/
class SimpleApplication : public Job
{
 public:
  SimpleApplication(TCPSocket* theSocket);
  // Constructor. The application is created by class TCP when a connection is
  // established.

  void doit();
  // Gets called when the application thread begins execution.
  // The SimpleApplication job is scheduled by TCP when a connection is
  // established.
  
 private:
  TCPSocket* mySocket;
  // Pointer to the application associated with this job.
};

#endif
/****************** END OF FILE tcpsocket.hh *********************************/
