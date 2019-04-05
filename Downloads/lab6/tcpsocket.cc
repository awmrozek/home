
#include "compiler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tcpsocket.hh"
#include "tcp.hh"
#include "threads.hh"


TCPSocket::TCPSocket(TCPConnection* theConnection) : myConnection(theConnection) {

  myReadSemaphore = Semaphore::createQueueSemaphore("read_semi",0);
  myWriteSemaphore = Semaphore::createQueueSemaphore("write_semi",0);

  eofFound = false;
}
TCPSocket::~TCPSocket() {

  delete myReadSemaphore;
  delete myWriteSemaphore;
}
byte* TCPSocket::Read(udword& theLength) {

  myReadSemaphore->wait(); // Wait for available data
  theLength = myReadLength;
  byte* aData = myReadData;
  myReadLength = 0;
  myReadData = NULL;
  return aData;// The caller must delete the data!
}
bool TCPSocket::isEof() {

  // Passive close

  return eofFound;
}
void TCPSocket::Write(const byte* theData, udword theLength) {

  myConnection->Send(theData, theLength);
  myWriteSemaphore->wait(); // Wait until the data is acknowledged
}
void TCPSocket::Close() {

  myConnection->AppClose();

}
void TCPSocket::socketDataReceived(byte* theData, udword theLength) {

  myReadData = new byte[theLength];
  memcpy(myReadData, theData, theLength);
  myReadLength = theLength;
  myReadSemaphore->signal();// Data is available
}
void TCPSocket::socketDataSent() {

  myWriteSemaphore->signal();
}
void TCPSocket::socketEof() {

  eofFound = true;
  myReadSemaphore->signal();
}



SimpleApplication::SimpleApplication(TCPSocket* theSocket) : mySocket(theSocket) {}
void SimpleApplication::doit()
  {
    cout << "Started Telnet server" << endl;

    udword aLength;
    byte* aData;
    bool done = false;
    while (!done && !mySocket->isEof())
    {
      aData = mySocket->Read(aLength);

      if (aLength > 0)
      {
        if (aData[0] == 'q'  && (aLength == 3 || aData[1] == '\r' ))
        {
          done = true;
        } else if((char)*aData == 's' && (aLength == 3 || aData[1] == '\r' )) {

          const char* big =
"                       _____\r\n"
"                   .d88888888bo.\r\n"
"                 .d8888888888888b.\r\n"
"                 8888888888888888b\r\n"
"                 888888888888888888\r\n"
"                 888888888888888888\r\n"
"                  Y8888888888888888\r\n"
"            ,od888888888888888888P\r\n"
"         .'`Y8P'```'Y8888888888P'\r\n"
"       .'_   `  _     'Y88888888b\r\n"
"      /  _`    _ `      Y88888888b   ____\r\n"
"   _  | /  \\  /  \\      8888888888.d888888b.\r\n"
"  d8b | | /|  | /|      8888888888d8888888888b\r\n"
" 8888_\\ \\_|/  \\_|/      d888888888888888888888b\r\n"
" .Y8P  `'-.            d88888888888888888888888\r\n"
"/          `          `      `Y8888888888888888\r\n"
"|                        __    888888888888888P\r\n"
" \\                       / `   dPY8888888888P'\r\n"
"  '._                  .'     .'  `Y888888P`\r\n"
"     `\"'-.,__    ___.-'    .-'\r\n"
"         `-._````  __..--' \r\n"
"             `````` \r\n"
"```````````````````````````````` .,;;'';:,`````````````````````````````````\r\n"
"`````````````````````````` `,'''''';'''''''''''``` ````````````````````````\r\n"
"```````````````````````` ;'''''''''''''''''''''';', ```````````````````````\r\n"
"```````````````````````''''''''''''''''''''''''''''';  `` `````````````````\r\n"
"````````````````` ``.';;'''''''''''''''''''''''''''';''````````````````````\r\n"
"```````````````````'''';''''''''''''''''''''''''''''''';'` ````````````````\r\n"
"``````````````` `+'''''''''''''''''''''''''''''''''''''''#. ```````````````\r\n"
"````````````````''''''''''''''''''''''''''''''''''''''';#+''```````````````\r\n"
"``````````` ` ,;''''''''''''''''';;''''''''''''''''''';+###''``````````````\r\n"
"`````````````''###################'''''''''''''''''''''#####;;.````````````\r\n"
"``````````` '''###################''''''''''''''''''''######+'',```````````\r\n"
"```````` ` ''''###################'''''''';''''''''''+#######'';;``````````\r\n"
"``````````''';'###################''''''';'''''''''''#########;';,`````````\r\n"
"`````````''''';;'''#####''''''####,         :''''''''##'###+##;'';`````````\r\n"
"````````,''''''''''#####''''`.####,            :'''''#+'###;##''''' ```````\r\n"
"````````;;'''''''''#####''   .####,              ,'''+;'###''#'''''' ``````\r\n"
"```` ``''''''''''''#####`    .####,                ''';'###'''''''';'``````\r\n"
"```` `'''''''''''''#####     .####,     `           `'';###''''''''';``````\r\n"
"``````'''''''''''''#####     .####,`###+@             ''###''''''''''' ````\r\n"
"`````''''''''''''''#####     .####'######+             '###''''''''''':````\r\n"
"````.''''''''''''''#####     .############              ###'''''''''''; ```\r\n"
"````'''''''''''''''#####     .############              ###'''''''''''';```\r\n"
"````'''''''''''''' #####     .#####  @####              ###''''''''''''' ` \r\n"
"```''''''''''''''  #####     .####,  @####              ###''''''''''''',``\r\n"
"`` '''''''''''''`  #####     .####,`,#####     :@@'     ##@';;'''''''''''``\r\n"
"``;'''''''''''''   #####     .####'''@####'  `######'   ##@ ;;''''''''''; `\r\n"
"``'''''''''''''    #####     .####'''#####';'######@#   ##@ ''''''''''''';`\r\n"
"` '''''''''''''    #####   ` ;####'''#####'''###. ####  ##@  '''''''''''''`\r\n"
"`,''''''''''''`    #####    ''####'''#####';+##@  @##@  ##@  '''''''''''''`\r\n"
" '''''''''''''     #####   '''####'''#####';####` @##@  ##@  `''''''''''''`\r\n"
"`'''''''''''''     #####  .;''####'''#####''####++###@  ##@   ;''''''''''';\r\n"
"`''''''''''''.     #####` ';''####'''#####''#########@  ##@   +''''''''''''\r\n"
"`''''''''''';      ##### ,''''####'''#####''#####+###@  ##@   ,''''''''''''\r\n"
".''''''''''''      ##### '''''####'''#####''####'',` `  ##@   `''''''''''''\r\n"
",''''''''''''      ##### ';'''####'''#####''####;;'     ##@    ''''''''''''\r\n"
";;'''''''''''      #####`'''''####'''#####''####;'###@  ##@    ''''''''''''\r\n"
";''''''''''''      #####,'''''####'''#####''####';####  ##@    ;'''''''''''\r\n"
"'''''''''''';      #####:';'''####'''#####';####;'###:  ##@    ''''''''''''\r\n"
"'''''''''''';      #####:'''''####'''#####'''########   ##@    ''''''''''''\r\n"
";''''''''''''      #####.'''''####'''#####';'+######+   ##@    ''''''''''''\r\n"
":''''''''''''            ;'''''''''''#####'''''####,    ##@    ''''''''''''\r\n"
",''''''''''''            ''''''''''''#####'''''';;'     ##@    ''''''''''''\r\n"
"`''''''''''';            ';''''''''''#####'''';;''`     ##@   `''''''''''''\r\n"
"`''''''''''';             ;''''''''''#####';''''''      ##@   ;''''''''''''\r\n"
"`'''''''''''':            '''''''''''#####'''';''.      ##@   '''''''''''''\r\n"
"`'''''''''';''             ''''''''''#####'''''''       ##@   ;''''''''''':\r\n"
"`''''''''''''+#####    ####+;;''##########;#####@       ##@  ,''''''''''''`\r\n"
"``''''''''''''#####    #####.;''#################.      ##@  '''''''''''''`\r\n"
"``;'''''''''''#####   ,#####  '############@#####@      ##@ `;''''''''''''`\r\n"
"``''''''''''''#####,  ######, `@########+##;######      ##@`''''''''''''', \r\n"
"``,'''''''''''#####'  @#####+  @####+#####`  #####      ##@`'''''''''''''``\r\n"
"` `'''''''''''#####@  ######@  #####`@####   #####      ###''''''''''''''``\r\n"
"```;'''''''''''####@  #######  ##### @####   #####     #####;'''''''''';```\r\n"
"``  '''''''''''#####  #######  ##### @####   #####    #######''''''''''' ``\r\n"
"````'''''''''''##### `#######  ####@ @####   #####   @#######+'''''''''.```\r\n"
"```` ;'''''''''##### :#######  ##### @####   #####   ###+'####;''''''''````\r\n"
"`````''''''''''#####'+#######,,####. @####   #####   ###''+###'''''';'`````\r\n"
"``````;''''''''+####;####,###+'####  @####   #####   ###''+###''''''''`````\r\n"
"````` ,'''''''''####'+### ###@@####  @####   #####  ;@##''+###''''''' `````\r\n"
"`````` '';''''';####+#### ####@####  @####   #####.';###''+###''''''.``````\r\n"
"````````''''''''#########'########@  @####   #####';;+##''+###''''''` `````\r\n"
"`````````'''''''#########'########'  @####   #####'';###''+###'''''````````\r\n"
"`````````.'';'''########''########`  @####`;'#####'''###'''###''';`````````\r\n"
"``````````,'''''+#######''+#######'';+####';'#####'''###'''###'''``````````\r\n"
"```````````:'''''#######'''#######'''#####'''#####'''###';###+''```````````\r\n"
"````````````,''''#######'''#######'''#####'''#####'''####'####; ```````````\r\n"
"`````````````.'''#######'';#######'''#####'''#####'';########'  ```````````\r\n"
"`````````````` ''#######'''######+'''#####'''+####''''######@``````````````\r\n"
"``````````````` '';'''''''';;;'';''''''''';''''''';''''+###,` `````````````\r\n"
"`````````````````.''''';''''''''';;'';'';''''''''''''''''' ````````````````\r\n"
"`````````````````` :''''''''''''''''''''''''''''''''''''```````````````````\r\n"
"```````````````````` ;''''''''''''''''''''''''''''''''.````````````````````\r\n"
"`````````````````````` ,';''''''''''''''''''''''';;'` `````````````````````\r\n"
"``````````````````````````;'''''''''''''''''''''', `` `````````````````````\r\n";

          mySocket->Write((const byte*)big,strlen(big));
        }else {
          mySocket->Write(aData, aLength);
        }
        delete[] aData;
      }
    }
    mySocket->Close();

    cout << "Stopped Telnet server" << endl;
  }
