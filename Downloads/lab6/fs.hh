/*!***************************************************************************
*!
*! FILE NAME  : fs.hh
*!
*! DESCRIPTION: simple filesystem
*!
*!***************************************************************************/

#ifndef fs_hh
#define fs_hh

/****************** INCLUDE FILES SECTION ***********************************/


/****************** CLASS DEFINITION SECTION ********************************/

/*****************************************************************************
*%
*% CLASS NAME   : FileSystem
*%
*% BASE CLASSES : None
*%
*% CLASS TYPE   : Singleton
*%
*% DESCRIPTION  : simple file system
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
struct ChangedFile{
  ChangedFile* next;
  ChangedFile* prev;
  char* path;
  char* name;
  byte* data;
  udword length;
};

class FileSystem {

 public:

  FileSystem();

  static FileSystem& instance();

  bool writeFile(const char *path,const char *name,byte *theData,udword theLength);
  // true if the write was done ie if enough memory is present

  byte *readFile(char *path,char *name,udword& theLength);
  // returns the ptr to and the length of a file..

private:

  static const byte myFileSystem[];
};

#endif

/****************** END OF FILE fs.hh *************************************/
