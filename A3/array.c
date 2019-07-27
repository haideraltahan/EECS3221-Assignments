/***********************************************************************/
/**      Author: Minas Spetsakis                                      **/
/**        Date: Jul. 2019                                            **/
/** Description: Assgn. III                                           **/
/***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "array.h"
#include "functions.h"
char* filename;

void set_filename(char* name) {
    filename = name;
}
/**********************************************************************/
/********************       O P E N  A R R A Y     ********************/
/**********************************************************************/
void open_array(char *filename, array_t *arrayp, int *sizep)
{				/* Opens array from file filename and */
    /* returns its pointer and size */
    /* size is the size of the file divided by the size of the array struct */
    /* pointer array is obtained with mmap */
    int fd = open(filename, O_CREAT | O_RDWR); //open the file
    struct stat sb; //store the size of the file here
    fstat(fd, &sb);
    array_t sizeArray[1];
    int size = sb.st_size / (sizeof(*sizeArray[1])); //get the number of elements
    *sizep = size; //return it to sizep
    array_t map = (array_t) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //mmap the file
    *arrayp = map; //pass the map back as the array
}

/**********************************************************************/
/********************     C L O S E  A R R A Y     ********************/
/**********************************************************************/
void close_array(array_t *arrayp, int size)
{				/* Unmaps array and sets it to NULL */
    munmap(arrayp, size); //not much to say here
    arrayp = NULL;

}

/**********************************************************************/
/********************    C R E A T E  A R R A Y    ********************/
/**********************************************************************/
void create_array(char *filename, int index, array_t* arrayp)
{				/* Creates a file with an array with index */
    /* elements, all with the valid member false. */
    /* It is fatal error if file exists */
    /* size is the size of the file divided by the size of the array struct */
    /* pointer array is obtained with mmap */
    if (access(filename, F_OK) != -1) //if the file could be found already, error
        fatalerr("File already exists", 1, "the file cannot already exist when --create is used.");
    int fd = open(filename, O_CREAT | O_RDWR); //open the file
    array_t sizeArray[1]; //gives us the size of an array_t struct
    int size = sizeof(*sizeArray[0]) * index; //the total size in bytes of the array
    array_t array = malloc(size); //create an array of the correct size
    ftruncate(fd, size); //shrink/grow the file to the right size
    for (int i = 0; i < index; i++)
        array[i].index = i; //fill the array with blank values except for their indices
    array_t map = (array_t) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    memcpy(map, array, size); //mmap the file and store the array in the file
    arrayp = &map; //pass the file back as the input array
}


/**********************************************************************/
/********************       S E T  E N T R Y       ********************/
/**********************************************************************/
void set_entry(array_t array, char *name, int index, float age)
{				/* Sets the elements of the index'ed */
    /* entry to name and age and the valid */
    /* member to true */
    array[index].valid = 1; //make this entry valid
    array[index].age = age; //update the age
    int i = 0; //set the name
    while (i < strlen(name)) {
        array[index].name[i] = name[i];
        i++;
    }
    array[index].name[i] = '\0'; //terminate string will null
    int fd = open(filename, O_CREAT | O_RDWR); //open the file and get its size
    struct stat sb;
    fstat(fd, &sb);
    array_t sizeArray[1];
    int size = sb.st_size;
    array_t map = (array_t) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    map = array; //set the mmap file to be the array with the newly updated element
}

/**********************************************************************/
/********************       G E T  E N T R Y       ********************/
/**********************************************************************/
void get_entry(array_t array, char **name, int index, float *age)
{				/* Returns the index'ed entry's name and age */
    /* if valid, fatal error o/w */
    if (array[index].valid != 1) //if the index is invalid, error
        fatalerr("Index is not valid", 1, "the index must be set first.");
    *age = array[index].age; //return the arguments accordingly
    *name = array[index].name;


}

/**********************************************************************/
/********************   D E L E T E   E N T R Y    ********************/
/**********************************************************************/
void delete_entry(array_t array, int index)
{				/* Sets the valid element of the index'ed */
    /* entry to false */
    array[index].valid = 0; //invalidate the index
    int fd = open(filename, O_CREAT | O_RDWR); //open the file and get its size
    struct stat sb;
    fstat(fd, &sb);
    int size = sb.st_size;
    array_t map = (array_t) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    map = array; //set the mmaped file to be the array with the newly invalidated index

}

/**********************************************************************/
/********************     P R I N T   A R R A Y    ********************/
/**********************************************************************/
void print_array(array_t array, int size)
{				/* Prints all entries with valid member true */
    /* using the same format as in the main */
    for (int i = 0; i < size; i++) { //loop over the array and print every entry with a valid index
        if (array[i].valid == 1)
            printf("index: %d, name: %s, age: %f\n", array[i].index, array[i].name, array[i].age);
    }

}
