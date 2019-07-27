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

/**********************************************************************/
/********************       O P E N  A R R A Y     ********************/
/**********************************************************************/
void open_array(char *filename, array_t *arrayp, int *sizep) {                /* Opens array from file filename and */
    /* returns its pointer and size */
    /* size is the size of the file divided by the size of the array struct */
    /* pointer array is obtained with mmap */
}

/**********************************************************************/
/********************     C L O S E  A R R A Y     ********************/
/**********************************************************************/
void close_array(array_t *arrayp, int size) {
    /* Unmaps array and sets it to NULL */
    if (munmap(arrayp, size) == -1)
    {
        perror("Error un-mmapping the file");
    }
}

/**********************************************************************/
/********************    C R E A T E  A R R A Y    ********************/
/**********************************************************************/
void create_array(char *filename, int index, array_t *arrayp) {
    /* Creates a file with an array with index */
    /* elements, all with the valid member false. */
    /* It is fatal error if file exists */
    /* size is the size of the file divided by the size of the array struct */
    /* pointer array is obtained with mmap */
    int fd, i, filesize;

    filesize = index * sizeof(array_t);

    if ((fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
        fatalerr("create_array", fd, "Could not create the file");
    }

    if (ftruncate(fd, filesize) == -1)
    {
        fatalerr("create_array", -1, "Could not 'stretch' the file");
    }

    arrayp = (array_t *) mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (arrayp == MAP_FAILED) {
        close(fd);
        fatalerr("create_array", -1, "Could not create mmap");
    }

    for (i = 0; i < index; ++i) {
        array_t temp = malloc(sizeof(array_t));
        if (temp == NULL)
            fatalerr("create_array", 0, "Out of memory\n");
        temp->valid = 0;
        arrayp[i] = temp;
    }

}


/**********************************************************************/
/********************       S E T  E N T R Y       ********************/
/**********************************************************************/
void set_entry(array_t array, char *name, int index, float age) {                /* Sets the elements of the index'ed */
    /* entry to name and age and the valid */
    /* member to true */

}

/**********************************************************************/
/********************       G E T  E N T R Y       ********************/
/**********************************************************************/
void get_entry(array_t array, char **name, int index,
               float *age) {                /* Returns the index'ed entry's name and age */
    /* if valid, fatal error o/w */

}

/**********************************************************************/
/********************   D E L E T E   E N T R Y    ********************/
/**********************************************************************/
void delete_entry(array_t array, int index) {                /* Sets the valid element of the index'ed */
    /* entry to false */

}

/**********************************************************************/
/********************     P R I N T   A R R A Y    ********************/
/**********************************************************************/
void print_array(array_t array, int size) {
    /* Prints all entries with valid member true */
    /* using the same format as in the main */
    int i;
    printf("out:\n");
    for (i = 0; i < size; ++i) {
        if(array[i].valid){
            printf("index: %d, ", array[i].index);
            printf("name: %s, ", array[i].name);
            printf("age: %.6f\n", array[i].age);
        }
    }
}
