/***********************************************************************/
/**      Author: Minas Spetsakis                                      **/
/**        Date: Jul. 2019                                            **/
/** Description: Assgn. III                                           **/
/***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "array.h"
#include "functions.h"

/***********************************************************************
                               M A I N
************************************************************************/
int main(int argc, char **argv)
{
  char *filename="namedb";
  int size=-1;
  char *name=NULL;
  array_t array=NULL;
  int index=-1;
  float age=-1.0;
  int i;
  int argv_i_len;

  i=1;
  while (i<argc) {
    argv_i_len = strlen(argv[i]);
    if (argv_i_len<3) argv_i_len=3;
    if (strncmp("--filename",argv[i],argv_i_len)==0) {
      if (i==argc)
	fatalerr(argv[0],0,"Insufficient arguments");
      filename = argv[++i];
      if (array!=NULL)
	close_array(&array, size);
    }
    else if (strncmp("--index",argv[i],argv_i_len)==0) {
      if (i==argc)
	fatalerr(argv[0],0,"Insufficient arguments");
      index = atoi(argv[++i]);
    }
    else if (strncmp("--name",argv[i],argv_i_len)==0) {
      if (i==argc)
	fatalerr(argv[0],0,"Insufficient arguments");
      if (strlen(name = argv[++i])>=MAXNAME)
	fatalerr(argv[0],0,"Name too long");
    }
    else if (strncmp("--age",argv[i],argv_i_len)==0) {
      if (i==argc)
	fatalerr(argv[0],0,"Insufficient arguments");
      age   = atof(argv[++i]);
    }
    else if (strncmp("--set",argv[i],argv_i_len)==0) {
      if (index<0 || age<0.0 || name==NULL)
	fatalerr(argv[0],0,"index, name or age undefined");
      if (array==NULL)
	open_array(filename,&array, &size);
      if (index>=size)
	fatalerr(argv[0],0,"index out of bounds");
      set_entry(array, name, index, age);
      name = NULL;
      index = -1;
      age = -1.0;
    }
    else if (strncmp("--get",argv[i],argv_i_len)==0) {
      if (index<0)
	fatalerr(argv[0],0,"index undefined");
      if (array==NULL)
	open_array(filename,&array, &size);
      if (index>=size)
	fatalerr(argv[0],0,"index out of bounds");
      get_entry(array, &name, index, &age);
      printf("index: %d, name: %s, age: %f\n", index, name, age);
    }
    else if (strncmp("--create",argv[i],argv_i_len)==0) {
      if (array!=NULL)
	close_array(&array, size);
      if (index<0)
	fatalerr(argv[0],0,"index undefined");
      size = index;
      create_array(filename, size, &array);
    }
    else if (strncmp("--print",argv[i],argv_i_len)==0) {
      if (array==NULL)
	open_array(filename,&array, &size);
      print_array(array, size-1);
    }
    else if (strncmp("--maxindex",argv[i],argv_i_len)==0) {
      if (array==NULL)
	open_array(filename,&array, &size);
      printf("Maximum index for array %s: %d\n",filename, size);
    }
    else if (strncmp("--delete",argv[i],argv_i_len)==0) {
      if (array==NULL)
	open_array(filename,&array, &size);
      if (index<0)
	fatalerr(argv[0],0,"index undefined");
      delete_entry(array,index);
    }
    else fatalerr(argv[i],0,"Invalid option");
    i++;
  }
  close_array(&array, size);
}
