/*!***************************************************************************
*!
*! FILE NAME  : tcp.cc
*!
*! DESCRIPTION: TCP, Transport control protocol
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
#include "timr.h"
}

#include "iostream.hh"
#include "tcp.hh"
#include "ip.hh"
#include "http.hh"


#ifdef D_TCP
#define trace cout
#else
#define trace if(false) cout
#endif


TCP::TCP() {

}
TCP& TCP::instance() {

  static TCP myInstance;
  return myInstance;
}
TCPConnection* TCP::getConnection(IPAddress& theSourceAddress,uword theSourcePort,uword theDestinationPort) {
  TCPConnection* aConnection = NULL;
  // Find among open connections
  uword queueLength = myConnectionList.Length();
  myConnectionList.ResetIterator();
  bool connectionFound = false;
  while ((queueLength-- > 0) && !connectionFound)
  {
    aConnection = myConnectionList.Next();
    connectionFound = aConnection->tryConnection(theSourceAddress,
                                                 theSourcePort,
                                                 theDestinationPort);
  }
  if (!connectionFound)
  {
    // << "Connection not found!" << endl;
    aConnection = NULL;
  }
  else
  {
  //  // << "Found connection in queue" << endl;
  }
  return aConnection;
}
TCPConnection* TCP::createConnection(IPAddress& theSourceAddress,uword theSourcePort,uword theDestinationPort,InPacket*  theCreator) {
  TCPConnection* aConnection =  new TCPConnection(theSourceAddress,
                                                  theSourcePort,
                                                  theDestinationPort,
                                                  theCreator);
  myConnectionList.Append(aConnection);
  return aConnection;
}
void TCP::deleteConnection(TCPConnection* theConnection) {
  myConnectionList.Remove(theConnection);
  delete theConnection;
}
bool TCP::acceptConnection(uword portNo) {

  return portNo == 7 || portNo == 80;
}
void TCP::connectionEstablished(TCPConnection *theConnection) {

  if(theConnection->serverPortNumber() == 7) {

    TCPSocket* aSocket = new TCPSocket(theConnection);
    theConnection->registerSocket(aSocket);
    Job::schedule(new SimpleApplication(aSocket));

  } else if(theConnection->serverPortNumber() == 80) {

    TCPSocket* aSocket = new TCPSocket(theConnection);
    theConnection->registerSocket(aSocket);
    Job::schedule(new HTTPServer(aSocket));
  }

}
TCPConnection::TCPConnection(IPAddress& theSourceAddress,uword theSourcePort,uword theDestinationPort,InPacket* theCreator):
        hisAddress(theSourceAddress),
        hisPort(theSourcePort),
        myPort(theDestinationPort) {
  // << "TCP connection created" << endl;
  myTCPSender = new TCPSender(this, theCreator);
  myState = ListenState::instance();
  mySocket = NULL;
  transmitQueue = NULL;
  queueLength = 0;
  sendNext = 0;
  sentUnAcked = 0;


  timer = new retransmitTimer(this,SEC_TICS);
}

TCPConnection::~TCPConnection() {
  delete timer;
  delete myTCPSender;
  delete mySocket;
}

bool TCPConnection::tryConnection(IPAddress& theSourceAddress,uword theSourcePort,uword theDestinationPort) {
  return (theSourcePort      == hisPort   ) &&
         (theDestinationPort == myPort    ) &&
         (theSourceAddress   == hisAddress);
}
void TCPConnection::Synchronize(udword theSynchronizationNumber) {

  myState->Synchronize(this,theSynchronizationNumber);
}
// Handle an incoming SYN segment
void TCPConnection::NetClose() {

  myState->NetClose(this);
}
// Handle an incoming FIN segment
void TCPConnection::AppClose() {

  myState->AppClose(this);
}
// Handle close from application
void TCPConnection::Kill() {

  myState->Kill(this);
}
// Handle an incoming RST segment, can also called in other error conditions
void TCPConnection::Receive(udword theSynchronizationNumber,
             byte*  theData,
             udword theLength) {

  myState->Receive(this,theSynchronizationNumber,theData,theLength);
}
// Handle incoming data
void TCPConnection::Acknowledge(udword theAcknowledgementNumber) {
  myState->Acknowledge(this,theAcknowledgementNumber);

}
// Handle incoming Acknowledgement
void TCPConnection::Send(const byte* theData,udword theLength) {
  myState->Send(this,theData,theLength);
}
uword TCPConnection::serverPortNumber() {

  return myPort;
}
void TCPConnection::registerSocket(TCPSocket* theSocket) {

  mySocket = theSocket;
}
//----------------------------------------------------------------------------
// TCPState contains dummies for all the operations, only the interesting ones
// gets overloaded by the various sub classes.

void TCPState::Kill(TCPConnection* theConnection) {
  // << "TCPState::Kill" << endl;
  TCP::instance().deleteConnection(theConnection);
}
void TCPState::Synchronize(TCPConnection* theConnection,udword s) {}
void TCPState::NetClose(TCPConnection* theConnection) {}
void TCPState::AppClose(TCPConnection* theConnection) {}
void TCPState::Receive(TCPConnection* theConnection,udword w,byte* data,udword len) {}
void TCPState::Send(TCPConnection* theConnection,const byte* data,udword len) {}
void TCPState::Acknowledge(TCPConnection* theConnection,udword w) {}
ListenState*
ListenState::instance()
{
  static ListenState myInstance;
  return &myInstance;
}

void ListenState::Synchronize(TCPConnection* theConnection,udword theSynchronizationNumber) {

  // << "ListenState::Synchronize" << endl;

  // Send ACK
  if(TCP::instance().acceptConnection(theConnection->myPort)) {

    TCP::instance().connectionEstablished(theConnection);

    // << "got SYN on ECHO port" << endl;
    theConnection->receiveNext = theSynchronizationNumber + 1;
    theConnection->receiveWindow = 8*1024;
    theConnection->sendNext = get_time();
    // Next reply to be sent.
    // Send a segment with the SYN and ACK flags set.
    theConnection->myTCPSender->sendFlags(FLAG_SYN | FLAG_ACK);
    // Prepare for the next send operation.
    theConnection->sendNext += 1;
    theConnection->sentUnAcked = theConnection->sendNext;
    // Change state
    theConnection->myState = SynRecvdState::instance();

  } else {

    // << "send RST..." << endl;

    // Send a segment with the RST flag set.
    theConnection->myTCPSender->sendFlags(FLAG_RST);
    TCP::instance().deleteConnection(theConnection);
  }

}

SynRecvdState*
SynRecvdState::instance() {
  static SynRecvdState myInstance;
  return &myInstance;
}

void SynRecvdState::Acknowledge(TCPConnection* theConnection,udword theAcknowledgementNumber) {

  // << "SynRecvdState::Acknowledge" << endl;


  theConnection->myState = EstablishedState::instance();

}

EstablishedState*
EstablishedState::instance(){
  static EstablishedState myInstance;
  return &myInstance;
}

void EstablishedState::NetClose(TCPConnection* theConnection) {
  // << "EstablishedState::NetClose" << endl;

  theConnection->receiveNext++;

  // Update connection variables and send an ACK
  theConnection->myTCPSender->sendFlags(FLAG_ACK);
  // Go to NetClose wait state, inform application
  theConnection->myState = CloseWaitState::instance();


  // Normally the application would be notified next and nothing
  // happen until the application calls appClose on the connection.
  // Since we don't have an application we simply call appClose here instead.

  // Simulate application Close...
  theConnection->AppClose();
}

void EstablishedState::Receive(TCPConnection* theConnection,
                          udword theSynchronizationNumber,
                          byte*  theData,
                          udword theLength)
{

    theConnection->mySocket->socketDataReceived(theData,theLength);
    theConnection->myTCPSender->sendFlags(FLAG_ACK);
}
void EstablishedState::Acknowledge(TCPConnection* theConnection, udword theSynchronizationNumber) {
    if(theConnection->transmitQueue != NULL)
    {
      udword sent = theSynchronizationNumber - theConnection->sentUnAcked;
      theConnection->sentUnAcked=theSynchronizationNumber;
      if( theConnection->sentMaxSeq != 0)
      {
        theConnection->sendNext = theConnection->sentMaxSeq;
        theConnection->sentMaxSeq = 0;
      }

      if(sent > theConnection->queueLength ) {
        cout << "You done goofed!, no new line for you";
        abort();
      }

      theConnection->transmitQueue += sent;
      theConnection->queueLength -= sent;

      if(theConnection->queueLength == 0 && theConnection->sentUnAcked == theConnection->sendNext)  {
        theConnection->transmitQueue = NULL;

        theConnection->timer->cancel();
        theConnection->mySocket->socketDataSent();
      } else {

        udword sentNotAcked = theConnection->sendNext - theConnection->sentUnAcked;

        if( theConnection->queueLength < sentNotAcked )
          cout << "You done goofed!, no new line for you";


        theConnection->myTCPSender->sendFromQueue();


      }
    }


}

void EstablishedState::Send(TCPConnection* theConnection, const byte* theData, udword theLength) {

  theConnection->transmitQueue = theData;
  theConnection->queueLength = theLength;
  theConnection->firstSeq = theConnection->sendNext;
  theConnection->sentUnAcked = theConnection->sendNext;
  theConnection->sentMaxSeq = 0;
  theConnection->timer->start();

  theConnection->myTCPSender->sendFromQueue();

}
void EstablishedState::AppClose(TCPConnection* theConnection)
{
  //theConnection->myTCPSender->sendFlags(FLAG_ACK);
  theConnection->myTCPSender->sendFlags(FLAG_FIN | FLAG_ACK );
  theConnection->myState = FinWait1State::instance();
}

CloseWaitState*
CloseWaitState::instance()
{
  static CloseWaitState myInstance;
  return &myInstance;
}

void CloseWaitState::AppClose(TCPConnection* theConnection)
{
  theConnection->myTCPSender->sendFlags(FLAG_FIN | FLAG_ACK);
  theConnection->myState = LastAckState::instance();
}

LastAckState*
LastAckState::instance()
{
  static LastAckState myInstance;
  return &myInstance;
}

void LastAckState::Acknowledge(TCPConnection* theConnection, udword theAcknowledgementNumber) {
  theConnection->mySocket->socketEof();
  Kill(theConnection);
}

FinWait1State*
FinWait1State::instance()
{
  static FinWait1State myInstance;
  return &myInstance;
}

void FinWait1State::Acknowledge(TCPConnection* theConnection,udword theAcknowledgementNumber) {
  theConnection->myState = FinWait2State::instance();
}

FinWait2State*
FinWait2State::instance()
{
  static FinWait2State myInstance;
  return &myInstance;
}

void FinWait2State::NetClose(TCPConnection* theConnection) {
  theConnection->sendNext+=1;
  theConnection->receiveNext+=1;
  theConnection->myTCPSender->sendFlags(FLAG_ACK);

  Kill(theConnection);

}



//----------------------------------------------------------------------------
//
TCPSender::TCPSender(TCPConnection* theConnection,
                     InPacket*      theCreator):
        myConnection(theConnection),
        myAnswerChain(theCreator->copyAnswerChain()) // Copies InPacket chain!
{
}

//----------------------------------------------------------------------------
//
TCPSender::~TCPSender()
{
  myAnswerChain->deleteAnswerChain();
}

void TCPSender::sendFlags(byte theFlags)
{
  udword totalSegmentLength = 20;

  // Decide on the value of the length totalSegmentLength.
  // Allocate a TCP segment.
  byte* anAnswer = new byte[totalSegmentLength + myAnswerChain->headerOffset() + 6];
  // Calculate the pseudo header checksum
  /*
  TCPPseudoHeader* aPseudoHeader = new TCPPseudoHeader(myConnection->hisAddress,totalSegmentLength);
  uword pseudosum = aPseudoHeader->checksum();
  delete aPseudoHeader;
  */
  TCPPseudoHeader pseudoHeader = TCPPseudoHeader(myConnection->hisAddress,totalSegmentLength);
  uword pseudosum = pseudoHeader.checksum();
  // Create the TCP segment.
  TCPHeader* aTCPHeader = (TCPHeader*)(anAnswer + myAnswerChain->headerOffset());

  aTCPHeader->sourcePort = HILO(myConnection->myPort);
  aTCPHeader->destinationPort = HILO(myConnection->hisPort);
  aTCPHeader->sequenceNumber = LHILO(myConnection->sendNext);
  aTCPHeader->acknowledgementNumber = LHILO(myConnection->receiveNext);
  aTCPHeader->headerLength = 0x50;
  aTCPHeader->flags = theFlags;
  aTCPHeader->windowSize = HILO(myConnection->receiveWindow);
  aTCPHeader->checksum = 0;
  aTCPHeader->urgentPointer = 0;



  // Calculate the final checksum.
  aTCPHeader->checksum = calculateChecksum(anAnswer + myAnswerChain->headerOffset(),totalSegmentLength,pseudosum);



  myAnswerChain->answer((byte*)aTCPHeader,totalSegmentLength);
  // Deallocate the dynamic memory

}

void TCPSender::sendData(const byte* theData,udword theLength)
{

  byte* anAswer = new byte[theLength + myAnswerChain->headerOffset() + sizeof(TCPHeader) + 6];

  if(anAswer == NULL)
  {
    cout << "could not allocate return packet" << endl;
    return;
  }
  //write data to end of headers
  memcpy(anAswer + myAnswerChain->headerOffset() + sizeof(TCPHeader),theData,theLength);

  TCPHeader* header = (TCPHeader*)(anAswer + myAnswerChain->headerOffset());

  header->sourcePort = HILO(myConnection->myPort);
  header->destinationPort = HILO(myConnection->hisPort);
  header->sequenceNumber = LHILO(myConnection->sendNext);
  header->acknowledgementNumber = LHILO(myConnection->receiveNext);
  header->headerLength = 0x50;
  header->flags = FLAG_ACK | FLAG_PSH;
  header->windowSize = HILO(myConnection->receiveWindow);
  header->checksum = 0;
  header->urgentPointer = 0;

  TCPPseudoHeader pseudoHeader = TCPPseudoHeader(myConnection->hisAddress,theLength + sizeof(TCPHeader));

  uword pseudosum = pseudoHeader.checksum();

  header->checksum = calculateChecksum((byte*)header,theLength + sizeof(TCPHeader),pseudosum);

  myAnswerChain->answer((byte*)header,theLength + sizeof(TCPHeader));
}
void TCPSender::sendFromQueue() {

  udword inFlight = myConnection->sendNext - myConnection->sentUnAcked;

  udword numBytesToSend = MIN(myConnection->myWindowSize,myConnection->queueLength);
  numBytesToSend-=(myConnection->sendNext - myConnection->sentUnAcked);

  if(numBytesToSend) {

    myConnection->timer->cancel();

    if(myConnection->sendNext < myConnection->sentMaxSeq) {

      numBytesToSend = MIN(numBytesToSend,1400);
    }

    udword offset = inFlight;

    while(numBytesToSend > 0) {
      udword sentBytes = ( numBytesToSend > 1400 ) ? 1400 : numBytesToSend;
      sendData(myConnection->transmitQueue + offset,sentBytes);
      myConnection->sendNext += sentBytes;
      numBytesToSend -= sentBytes;
      offset += sentBytes;
    }
    myConnection->timer->start();
  }

}



//----------------------------------------------------------------------------
//
TCPInPacket::TCPInPacket(byte*           theData,
                         udword          theLength,
                         InPacket*       theFrame,
                         IPAddress&      theSourceAddress):
        InPacket(theData, theLength, theFrame),
        mySourceAddress(theSourceAddress) {

}


void TCPInPacket::decode() {
  const TCPHeader* header = (const TCPHeader*)myData;

  mySourcePort = HILO(header->sourcePort);
  myDestinationPort = HILO(header->destinationPort);
  mySequenceNumber = LHILO(header->sequenceNumber);
  myAcknowledgementNumber = LHILO(header->acknowledgementNumber);

  //only cares abouit the leftmost 4 bits in a byte for the len.
  udword len = myLength - 4*(header->headerLength >> 4);

  if(len >= 0) {


    TCPConnection* connection = TCP::instance().getConnection(mySourceAddress,mySourcePort,myDestinationPort);

    if(connection == NULL) {

      connection = TCP::instance().createConnection(mySourceAddress,mySourcePort,myDestinationPort,this);
      connection->myWindowSize = HILO(header->windowSize);

      if(header->flags & FLAG_SYN) {
        connection->Synchronize(mySequenceNumber);
      } else {
        connection->Kill();
      }

    } else if(connection->receiveNext == mySequenceNumber) {
      connection->myWindowSize = HILO(header->windowSize);

      if(header->flags & FLAG_ACK) {
        if(len > 0) {
          connection->receiveNext = mySequenceNumber + len;
          connection->Receive(myAcknowledgementNumber,myData + sizeof(TCPHeader),len);
        }

        connection->Acknowledge(myAcknowledgementNumber);

      }
      if(header->flags & FLAG_FIN) {

        connection->NetClose();
      }
      if(header->flags & FLAG_RST) {

        connection->Kill();
      }

    }

  }

}
void TCPInPacket::answer(byte* theData, udword theLength) {

    theData -= sizeof(TCPHeader);

    TCPHeader* header = (TCPHeader*)theData;

    header->sourcePort = HILO(myDestinationPort);
    header->destinationPort = HILO(mySourcePort);
    header->sequenceNumber = LHILO(myAcknowledgementNumber);
    header->acknowledgementNumber = LHILO(mySequenceNumber);
    header->headerLength = 0x50;
    header->flags = FLAG_ACK | FLAG_PSH;
    header->windowSize = HILO(8*1024);
    header->checksum = 0;
    header->urgentPointer = 0;

    TCPPseudoHeader pseudoHeader = TCPPseudoHeader(mySourceAddress,theLength);

    uword pseudosum = pseudoHeader.checksum();

    header->checksum = calculateChecksum(theData,theLength,pseudosum);

    myFrame->answer(theData,theLength + sizeof(TCPHeader));
}
uword TCPInPacket::headerOffset() {

  return myFrame->headerOffset() + sizeof(TCPHeader);
}
InPacket* TCPInPacket::copyAnswerChain() {

  return myFrame->copyAnswerChain();
}

//----------------------------------------------------------------------------
//
TCPPseudoHeader::TCPPseudoHeader(IPAddress& theDestination,
                                 uword theLength):
        sourceIPAddress(IP::instance().myAddress()),
        destinationIPAddress(theDestination),
        zero(0),
        protocol(6)
{
  tcpLength = HILO(theLength);
}

//----------------------------------------------------------------------------
//
uword TCPPseudoHeader::checksum() {

  return calculateChecksum((byte*)this, 12);
}

retransmitTimer::retransmitTimer(TCPConnection* theConnection,Duration retransmitTime) : myConnection(theConnection),myRetransmitTime(retransmitTime) {

}
void retransmitTimer::start() {

  this->timeOutAfter(myRetransmitTime);
}
void retransmitTimer::cancel() {

  this->resetTimeOut();
}

void retransmitTimer::timeOut() {
  myConnection->sentMaxSeq = myConnection->sendNext;
  myConnection->sendNext = myConnection->sentUnAcked;
  myConnection->myTCPSender->sendFromQueue();
}

/****************** END OF FILE tcp.cc *************************************/
