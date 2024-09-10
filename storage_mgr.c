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

//WORK DONE BY SABARISH RAJA RAMESH RAJA (A20576363)

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
    FILE *closefile = (FILE *)fHandle ->mgmtInfo; // getting the file pointer from mgmtInfo
    if(fclose(closefile) == 0){ // Close the file successfully
        return RC_OK;
    }
    else{
        return RC_FILE_NOT_CLOSED; // Return an appropriate error code
    }
}

//Destroying a page file
extern RC destroyPageFile (char *fileName){
    if(remove(fileName) == 0){ // If the file is removed successfully
        return RC_OK;
    } else {
        return RC_FILE_NOT_FOUND; // Return an error if the file could not be removed
    }
}

/*********************** READ OPERATIONS *********************/
extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
    //Initially, checking if the page number is valid:
    if(pageNum<0 || pageNum > fHandle->totalNumPages ){
        //If page doesn't exist
        return RC_READ_NON_EXISTING_PAGE;
    }
    //Creating a pointer for file using the additional information
    FILE *readblockfile = (FILE *)fHandle->mgmtInfo;
    //Steps => Get the File from fHandle -> Seek the page block using offset
    long offset = pageNum * PAGE_SIZE;
    //Move the pointer to current page
    int seek_pointer = fseek(readblockfile, offset, SEEK_SET);
    if(seek_pointer!=0){
        return RC_READ_NON_EXISTING_PAGE;
    }
    //Goal: Read a specific page from a file into memory
    size_t read = fread(memPage, sizeof(char), PAGE_SIZE, readblockfile);
    //SYNTAX: FREAD(VOID *PTR(destination where data is stored), 
    // SIZE_T SIZE (size of element to read), 
    // SIZE_T COUNT(number of elements to read), 
    // FILE *STREAM (file pointer from which data will be read))
    if(read<PAGE_SIZE){
        return RC_READ_NON_EXISTING_PAGE;
    }
    // Updatr the current page position to this page
    fHandle->curPagePos = pageNum;
    return RC_OK;

}

/*getBlockPos's responsibility to get the page's position from a file*/
extern int getBlockPos (SM_FileHandle *fHandle){
    //Constraints: Check if the files are present in the fHandle data structure and
    // Check if the current page is in a valid range i.e. between 0 and the total number of pages.
    if(fHandle == NULL || fHandle->mgmtInfo == NULL){
        return RC_FILE_HANDLE_NOT_INIT;
    }
    if(fHandle->curPagePos <1 || fHandle->curPagePos >= fHandle->totalNumPages){
        return RC_READ_NON_EXISTING_PAGE;
    }
    return fHandle->curPagePos;
}

//Reading the first Block of the file
extern RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    //Idea: Call the readBlock() function by passing the first page's index i.e. 0
    return readBlock(0, fHandle, memPage);
}

//Reading the Last Block of the File
extern RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    int lastPage = fHandle->totalNumPages-1;
    return readBlock(lastPage, fHandle, memPage);
}

//Reading the previous block from the current block 
extern RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    int previousPage = fHandle->curPagePos - 1;
    //Check if the page number is not below 0
    if(previousPage <= 0){
        return RC_READ_NON_EXISTING_PAGE;
    }
    return readBlock(previousPage, fHandle, memPage);
}

//Reading the current block from file
extern RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    int currentPage = fHandle->curPagePos;
    return readBlock(currentPage, fHandle, memPage);
}

//Reading the Next Block from file
extern RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    int nextPage = fHandle->curPagePos + 1;
    //The next block fo file should not be more than total number of pages
    if(nextPage>=fHandle->totalNumPages){
        return RC_READ_NON_EXISTING_PAGE;
    }
    return readBlock(nextPage, fHandle, memPage);
}

/*********************** WRITE OPERATIONS *********************/

// WORK BY RUDRA PATEL 

extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Check if the page number is valid
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages) {
        return RC_WRITE_FAILED;
    }

    // Create a file pointer using the information in fHandle
    FILE *file = (FILE *) fHandle->mgmtInfo;

    // Move the pointer to the correct page location
    long offset = pageNum * PAGE_SIZE;
    if (fseek(file, offset, SEEK_SET) != 0) {
        return RC_WRITE_FAILED;
    }

    // Write the block of memory to the file
    size_t write_size = fwrite(memPage, sizeof(char), PAGE_SIZE, file);
    if (write_size < PAGE_SIZE) {
        return RC_WRITE_FAILED;
    }

    // Update the current page position in the file handle
    fHandle->curPagePos = pageNum;

    return RC_OK;
}

extern RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Ensure that the current page position is valid
    if (fHandle->curPagePos < 0 || fHandle->curPagePos >= fHandle->totalNumPages) {
        return RC_WRITE_FAILED;
    }

    // Use writeBlock to write to the current page
    return writeBlock(fHandle->curPagePos, fHandle, memPage);
}

extern RC appendEmptyBlock(SM_FileHandle *fHandle) {
    // Allocate memory for an empty page and set it to 0
    SM_PageHandle emptyPage = (SM_PageHandle) calloc(PAGE_SIZE, sizeof(char));
    if (emptyPage == NULL) {
        return RC_WRITE_FAILED;  // Memory allocation failed
    }

    // Retrieve the file pointer from the file handle (stored in mgmtInfo)
    FILE *file = (FILE *) fHandle->mgmtInfo;

    // Move the file pointer to the end of the file
    if (fseek(file, 0L, SEEK_END) != 0) {
        free(emptyPage);
        return RC_FILE_NOT_FOUND;  // File seek error
    }

    // Write the empty page (filled with zero bytes) to the file
    size_t write_code = fwrite(emptyPage, sizeof(char), PAGE_SIZE, file);
    if (write_code != PAGE_SIZE) {
        free(emptyPage);
        return RC_WRITE_FAILED;  // Error in writing to the file
    }

    // Update the file handle's total number of pages
    fHandle->totalNumPages++;

    // Free the allocated memory for the empty page
    free(emptyPage);

    // Return success
    return RC_OK;
}

/**********Verifying if the code works by executing all the functions ************/
int main() {
    // Initialize storage manager
    initStorageManager();

    // Declare the file handle and page handle (memory for the page)
    SM_FileHandle fHandle;
    SM_PageHandle memPage = (SM_PageHandle) malloc(PAGE_SIZE);

    // Step 1: Create a new page file
    char *fileName = "test_pagefile.bin";
    RC rc = createPageFile(fileName);
    if (rc != RC_OK) {
        printf("Failed to create the page file. Error: %d\n", rc);
        return rc;
    }
    printf("Page file created successfully!\n");

    // Step 2: Open the created page file
    rc = openPageFile(fileName, &fHandle);
    if (rc != RC_OK) {
        printf("Failed to open the page file. Error: %d\n", rc);
        return rc;
    }
    printf("Page file opened successfully!\n");

    // Step 3: Write data to the first (current) block
    strcpy(memPage, "This is a test string for the first block.");
    rc = writeCurrentBlock(&fHandle, memPage);
    if (rc != RC_OK) {
        printf("Failed to write to the current block. Error: %d\n", rc);
        return rc;
    }
    printf("Data written to the current block successfully!\n");

    // Step 4: Read data back from the first block to verify
    memset(memPage, 0, PAGE_SIZE);  // Clear the memory page
    rc = readFirstBlock(&fHandle, memPage);
    if (rc != RC_OK) {
        printf("Failed to read the first block. Error: %d\n", rc);
        return rc;
    }
    printf("Data read from the first block: %s\n", memPage);

    // Step 5: Close the file
    rc = closePageFile(&fHandle);
    if (rc != RC_OK) {
        printf("Failed to close the page file. Error: %d\n", rc);
        return rc;
    }
    printf("Page file closed successfully!\n");

    // Step 6: Clean up by destroying the file
    rc = destroyPageFile(fileName);
    if (rc != RC_OK) {
        printf("Failed to destroy the page file. Error: %d\n", rc);
        return rc;
    }
    printf("Page file destroyed successfully!\n");

    // Free allocated memory for the page
}

