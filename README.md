Storage Manager
---------------------------------------

Team members
---------------------------------------
1) Sabarish Raja

2) Rudra Hirenkumar Patel

3) Enoch Ashong

Aim
----------------------------------------------------------------------------------------
The objective of this assignment is to develop a storage manager, which is a module that can read blocks from a file on disk into memory and write blocks from memory back to the disk. The storage manager operates on pages (blocks) of a fixed size (PAGE_SIZE). Functions have been provided to create, open, and close files, as well as to read, write, and append pages within a file. The read functions also support reading the current block, the next block, the previous block, and more.

Functions
----------------------------------------------------------------------------------------
+ createPageFile()
+ openPageFile()
+ closePageFile()
+ destroyPageFile()
+ readBlock()
+ getBlockPos()

1. createPageFile()
----------------------------------------------------------------------------------------
The createPageFile function is designed to create a new file and initialize it with an empty page of data. It begins by attempting to open the specified file in write mode using fopen. If the file cannot be opened, the function returns an error code RC_FILE_NOT_FOUND. Next, the function allocates memory for one page using calloc, which ensures the memory is initialized to zero. If memory allocation fails, the file is closed, and the function returns an error code RC_WRITE_FAILED. The function then attempts to write the empty page to the file using fwrite. If the write operation does not successfully write the entire page, the allocated memory is freed, the file is closed, and an error code RC_WRITE_FAILED is returned. If all operations succeed, the function frees the allocated memory, closes the file, and returns RC_OK to indicate successful file creation and initialization.

2. openPageFile()
----------------------------------------------------------------------------------------
The openPageFile function is responsible for opening an existing page file and initializing the file handle structure with relevant information. It begins by attempting to open the file in read and write mode using fopen. If the file cannot be opened, the function returns an error code RC_FILE_NOT_FOUND. Upon successfully opening the file, the function sets the fileName field of the SM_FileHandle structure to the provided file name and initializes the current page position to the first page (index 0). To determine the total number of pages in the file, the function moves the file pointer to the end of the file using fseek and retrieves the file size using ftell. The total number of pages is then calculated by dividing the file size by the constant PAGE_SIZE. Finally, the file pointer is stored in the mgmtInfo field of the SM_FileHandle structure, and the function returns RC_OK to indicate successful file opening and initialization.

3. closePageFile()
----------------------------------------------------------------------------------------
The closePageFile function is responsible for closing an open page file and handling any associated cleanup. It retrieves the file pointer from the mgmtInfo field of the SM_FileHandle structure, which was previously set when the file was opened. The function then attempts to close the file using fclose. If the file is successfully closed, the function returns RC_OK to indicate success. However, if the file fails to close, the function sets the file pointer to NULL to ensure it no longer references the closed file. Note that while the function handles closing the file, it does not return an explicit error code for the failure to close the file; it merely sets the pointer to NULL.

4. destroyPageFile()
----------------------------------------------------------------------------------------
The destroyPageFile function is designed to delete a specified page file from the filesystem. It attempts to remove the file using the remove function, which deletes the file named fileName. If the file is successfully deleted, the function returns RC_OK to indicate that the operation was successful. However, if the file could not be deleted, the function does not return an error code or handle the failure explicitly, so it implicitly assumes the operation failed if remove does not return zero.

5. readBlock()
----------------------------------------------------------------------------------------
The readBlock function is designed to read a specific page from a page file into memory. It first verifies that the requested page number is valid by checking if it falls within the range of existing pages in the file; if not, it returns RC_READ_NON_EXISTING_PAGE. The function then retrieves the file pointer from the mgmtInfo field of the SM_FileHandle structure and calculates the file offset for the desired page by multiplying the page number by PAGE_SIZE. It uses fseek to move the file pointer to this offset. If fseek fails, it returns RC_READ_NON_EXISTING_PAGE. The function then reads the data for the page into the provided memory buffer (memPage) using fread. If fread does not read the full page size, it indicates a read error and returns RC_READ_NON_EXISTING_PAGE. Finally, if all operations are successful, the function updates the curPagePos in the file handle to the current page number and returns RC_OK to indicate successful reading.

6. getBlockPos()
----------------------------------------------------------------------------------------
The getBlockPos function is used to retrieve the current page position from a file handle structure, but only if certain conditions are met. First, it checks if the fHandle pointer or its mgmtInfo field is NULL, which would indicate that the file handle is not properly initialized; in this case, it returns RC_FILE_HANDLE_NOT_INIT. Next, it verifies that the current page position (curPagePos) is within a valid range—between 0 and the total number of pages in the file. If curPagePos is out of range, it returns RC_READ_NON_EXISTING_PAGE. If both checks pass, the function returns the current page position, indicating that it is valid and can be accessed.











