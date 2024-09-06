#include "storage_mgr.h" //Adding header file for the interface
#include "dberror.h" //Error code header file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Initially, we have to initialize the storage manager
extern void initStorageManager(void){
    printf("The storage manager is initialized!!");
}

/*********************** FILE OPERATIONS *********************/

//Creating a page file
extern RC createPageFile (char *fileName){
    FILE *fileopen = fopen(fileName, "w"); //Opening the file to write
    //We have to check if the file is open or not
    if(fileopen == NULL){
        return RC_FILE_NOT_FOUND;   //If not openeed, return ERROR
    }

    //Create an empty page and allocate some memory for one page
    char *newpage = (char *)calloc(PAGE_SIZE, sizeof(char));
    //In case the memory is not allocated, 
    if(!newpage){
        fclose(fileopen);
        return RC_WRITE_FAILED;
    }

    //Now, we check if writing is possible in the empty page
    //SYNTAX of fwrite ->  size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
    size_t write_code = fwrite(newpage, sizeof(char), PAGE_SIZE, fileopen);

    //CHECK IF THE WRITE OPERATION SUCCEEDED
    if(write_code != PAGE_SIZE){
        free(newpage); //Freeing the allocated memory
        fclose(fileopen);  //CLOSE THE FILE
        return RC_WRITE_FAILED;
    }

    //FREE ALL THE ALLOCATED MEMRY after writing
    free(newpage);
    fclose(fileopen);
    return RC_OK;
}

//OPEN AN EXISTING PAGE FILE
extern RC openPageFile (char *fileName, SM_FileHandle *fHandle){
    FILE *file = fopen(fileName, "r+");  //Opening file for reading and writing
    if(file == NULL){
        return RC_FILE_NOT_FOUND;
    }
    //Set file handle field to store information about file
    //in the SM_FileHandle
    fHandle-> fileName = fileName; //Storing file name
    fHandle-> curPagePos = 0; //Setting the current page to 1st page

    //We can calculate the total number of pages in the file by getting file size
    fseek(file, 0L, SEEK_END); //Moving to end of the file
    long fileSize = ftell(file); //Get size of the file
    fHandle -> totalNumPages = (int) (fileSize/PAGE_SIZE);

    fHandle->mgmtInfo = file;
    return RC_OK;

}

//CLOSING AN OPEN PAGE
extern RC closePageFile (SM_FileHandle *fHandle){
    FILE *closefile = (FILE *)fHandle ->mgmtInfo; //getting the file pointer from mgmtInfo
    if(fclose(closefile) == 0){                   //Close the file in the condition itselg
        return RC_OK;
    }
    else{
        closefile = NULL;
    }
}

//Destroying a page file
extern RC destroyPageFile (char *fileName){
    if(remove(fileName) == 0){
        return RC_OK;
    }
}

