/*!***************************************************************************
*!
*! FILE NAME  : fs.cc
*!
*! DESCRIPTION: FileSystem
*!
*!***************************************************************************/

/****************** INCLUDE FILES SECTION ***********************************/

#include "compiler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iostream.hh"
extern "C"
{
#include "system.h"
}

#include "fs.hh"

#define D_fs
#ifdef D_fs
#define trace cout
#else
#define trace if(false) cout
#endif
/****************** FileSystem DEFINITION SECTION *************************/

//----------------------------------------------------------------------------
//



static ChangedFile* firstChangedFile = NULL;

FileSystem::FileSystem() {

}

//----------------------------------------------------------------------------
//
FileSystem& FileSystem::instance() {

  static FileSystem myInstance;
  return myInstance;
}

bool FileSystem::writeFile(const char *path,const char *name,byte *theData,udword theLength) {

  udword pathLen = strlen(path);
  udword nameLen = strlen(name);

  ChangedFile* cf = (ChangedFile*)(theData - sizeof(ChangedFile) - pathLen - nameLen - 2);

  cf->path = (char*)(cf) + sizeof(ChangedFile);
  cf->name = (char*)(cf) + sizeof(ChangedFile) + pathLen + 1;
  cf->data = theData;
  cf->length = theLength;


  strcpy(cf->path,path);
  strcpy(cf->name,name);

  for(ChangedFile* f = firstChangedFile; f != NULL; f = f->next) {

    if(strcmp(path,f->path) == 0 && strcmp(name,f->name) == 0) {
      cf->next = f->next;
      cf->prev = f->prev;
      if(f->prev != NULL) {
        f->prev->next = cf;
      } else {
        firstChangedFile = cf;
      }
      if(f->next != NULL) f->next->prev = cf;
    //  cout << "delete[] " << hex << (udword)((char*)f) << dec << endl;
      delete[] f->data;
      delete[] (char*)f;
      return true;
    }

  }

  // Insert first in list
  if(firstChangedFile != NULL) firstChangedFile->prev = cf;
  cf->prev = NULL;
  cf->next = firstChangedFile;
  firstChangedFile = cf;

  return true;
}

typedef struct lzhead{
        unsigned char           HeadSiz,HeadChk,HeadID[5]; // 0 1 2
        unsigned char           PacSiz[4];                 // 7
        unsigned char           OrgSiz[4];                 // 11
        unsigned char           Ftime[4];                  // 15
        unsigned char           Attr,Level;                // 17
        unsigned char           name_length;               // 18
        unsigned char           Fname[1];                  // 19
} LzHead;

const byte FileSystem::myFileSystem[]={
#include "lhafile.bin"
};

byte *FileSystem::readFile(char *path,char *name,udword& theLength) {

  for(ChangedFile* f = firstChangedFile; f != NULL; f = f->next) {

    if(strcmp(path,f->path) == 0 && strcmp(name,f->name) == 0) {
      theLength = f->length;
      return f->data;
    }

  }


  int file_size=sizeof(FileSystem::myFileSystem);
  int curr_size=0;
  int curr_file_size=0;

  while((curr_size<(file_size-16))&&(curr_size>=0))
  {
    LzHead *header=(LzHead *)&FileSystem::myFileSystem[curr_size];
    char *file_path;
    udword the_file_size;

    curr_size+=header->HeadSiz+2;

    curr_file_size=header->PacSiz[0];
    curr_file_size+=header->PacSiz[1]<<8;
    curr_file_size+=header->PacSiz[2]<<16;
    curr_file_size+=header->PacSiz[3]<<24;

    the_file_size=curr_file_size;

    // check if it is the correct file.
    if ((!strncmp(name,header->Fname,header->name_length))&&
	(header->name_length))
      {
	bool endCheck=false;

        byte *file_ptr=&FileSystem::myFileSystem[curr_size];


	//may have found it check path

	if ((header->HeadSiz>27)&&(header->Level==1)&&
	    (header->Fname[header->name_length+17]==2))
	  {
	    byte *file_path=&header->Fname[header->name_length+18];
	    byte *file_path2=file_path;
	    udword path_len;

	    endCheck=true;

	    //hitta slut
	    do
	      while(*(file_path2++)!=0xff);
	    while (*file_path2!=7);

	    the_file_size-=((int)(file_path2-file_ptr))+9;
	    file_ptr=file_path2+9;


	    path_len=file_path2-file_path;
	    if (!strncmp(path,file_path,path_len))
	      {
		if (strlen(path)==path_len)
		  {
		    endCheck=false;
		  }
	      }
	  }
	else
	  {
	    file_ptr += 19;
            the_file_size -= 19;
	    if (*path)
	      {
		endCheck=true;
	      }
	  }

	if (!endCheck)
	  {
	    //found it!

	    // calc size and ptr
	    theLength=the_file_size;
	    return file_ptr;
	  }
      }
    curr_size+=curr_file_size;
  }
  return 0;
}


/****************** END OF FILE fs.cc *************************************/
