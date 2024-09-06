#include <sys/stat.h> // Header file that includes functions and macros related to file attributes and file status.
#include "storage_mgr.h"
#include <stdio.h>

RC createPageFile (char *filename)
{
    FILE *file;
	int i;
	char response;
	file = fopen(filename, "r"); //opens the file to read.
	
    if(file != NULL) // If the file exists, it will be opened. If not, fopen returns NULL
    {
        printf("File Already Exists!! \n Do you want to Over-Write the file (Y/N): ");
        scanf("%c",&response);
        fclose(file);
    }

    if(response == 'Y' || file == NULL)
    {
        file = fopen(filename, "w"); // opens the file to write 
        fseek(file , 0 , SEEK_SET); // seeking the start of the file
    
        for(i = 0; i < PAGE_SIZE; i++)
        {
            fwrite("\0",1, 1,file);
            fseek(file,0,SEEK_END);
        }
        
        return RC_OK;
    }
    else
    {
        return RC_FILE_PRESENT;
    }

}

int main() // testing to see if the function creates a file
{ 
    RC result = createPageFile("fileONE.txt");
    if (result == RC_OK) {
        printf("File created successfully.\n");
    } else if (result == RC_FILE_PRESENT) {
        printf("File already exists and was not overwritten.\n");
    } else {
        printf("An error occurred.\n");
    }
    return 0;
}

/******************************** READ OPERATIONS FOR A SPECIFIC PAGE FROM FILE ON TO A MEMORY ******************************/
// readBlock will read the block at position pageNum which is pre-defined
extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
    // memPage parameter is a pointer to a block of memory where data read from the file will be stored.
    // *fhandle -> Pointer to an instance of SM_FileHandle
    
    //First, we'll have to verify that the pageNum is non-negative and it does not exceed the total number of pages 
    
}