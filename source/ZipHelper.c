#include "ZipHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minizip/zip.h"
#include "minizip/unzip.h"

#define dir_delimter '/'
#define MAX_FILENAME 256
#define READ_SIZE 9216

int unzipAndLoad(char *filename, int (*fileCallback)(char *filename), char *expectedComment, int keepFiles) {
    // Open the zip file
    unzFile *zipfile = unzOpen(filename);
    if (zipfile == NULL) {
        return 1; // Error: ZipFile could not be opened.
    }
    
    // Get info about the zip file
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info ) != UNZ_OK) {
        unzClose(zipfile);
        return 2; // Error: Could not read global info
    }
    
    if(expectedComment!=NULL) {
        char *buffer = malloc(global_info.size_comment+1);
        if(buffer==NULL) {
            unzClose(zipfile);
            return 3; // Error: Could not read global comment
        }
        
        if (unzGetGlobalComment(zipfile, buffer, global_info.size_comment+1) < 0) {
            free(buffer);
            unzClose(zipfile);
            return 3; // Error: Could not read global comment
        }
        
        if (strcmp(expectedComment, buffer)!=0) {
            free(buffer);
            unzClose(zipfile);
            return 4; // Error: Global comment did not have expected value
        }
        
        free(buffer);
    }
    
    // Buffer to hold data read from the zip file.
    void *read_buffer = malloc(READ_SIZE);
    if(read_buffer==NULL) {
        unzClose(zipfile);
        return 5; // Error: Could not allocate read buffer
    }

    // Loop to extract all files
    uLong i;
    for (i = 0; i < global_info.number_entry; ++i) {
        // Get info about current file.
        unz_file_info file_info;
        char filename[MAX_FILENAME];
        if (unzGetCurrentFileInfo(zipfile, &file_info, filename, MAX_FILENAME, NULL, 0, NULL, 0 ) != UNZ_OK) {
            free(read_buffer);
            unzClose(zipfile);
            return 6; // Error: Could not read file info
        }

        // Check if this entry is NOT a directory or file.
        const size_t filename_length = strlen(filename);
        if (filename[ filename_length-1 ] != dir_delimter){
            if (unzOpenCurrentFile( zipfile ) != UNZ_OK) {
                free(read_buffer);
                unzClose(zipfile);
                return 7;
            }

            // Open a file to write out the data.
            FILE * out = fopen(filename, "wb");
            if (out == NULL) {
                free(read_buffer);
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return 8;
            }

            int error = UNZ_OK;
            do {
                error = unzReadCurrentFile(zipfile, read_buffer, READ_SIZE);
                if ( error < 0 ) {
                    //printf("error %d\n", error);
                    free(read_buffer);
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    fclose(out);
                    remove(filename);
                    return 9;
                }

                // Write data to file.
                if (error > 0) {
                    fwrite(read_buffer, error, 1, out); // You should check return of fwrite...
                }
            } while (error > 0);

            fclose(out);
            
            //run callback
            if((*fileCallback)(filename) != 0) {
                free(read_buffer);
                unzClose(zipfile);
                remove(filename);
                return 10; // Error: Callback error
            }
            
            if(keepFiles==ZIPHELPER_CLEANUP_FILES) {
                remove(filename);
            }
        }

        unzCloseCurrentFile(zipfile);

        // Go the the next entry listed in the zip file.
        if (( i+1 ) < global_info.number_entry) {
            if (unzGoToNextFile( zipfile ) != UNZ_OK) {
                free(read_buffer);
                unzClose(zipfile);
                return 11;
            }
        }
    }
    
    free(read_buffer);
    unzClose(zipfile);
    
    return 0;
}

int zipFiles(char *filename, char **files, int fileCount, int mode, char *comment) {
    // Set mode
    int zipMode = mode==ZIPHELPER_ADD ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE;
    FILE *testFile = fopen(filename, "r");
    if(testFile!=NULL) {
        fclose(testFile);
    } else {
        zipMode = APPEND_STATUS_CREATE;
    }
    
    // Open the zip file
    zipFile *zipfile = zipOpen(filename, zipMode);
    if (zipfile == NULL) return 1; // Error: ZipFile could not be opened.
    
    // Buffer to hold data read from the files.
    void *read_buffer = malloc(READ_SIZE);
    if(read_buffer==NULL) {
        // Error: Could not allocate read buffer
        zipClose(zipfile, "");
        return 2;
    }
    
    // Loop all files to add
    for(int i = 0; i < fileCount; i++) {
        // Open a zipfile to write out the data.
        if (zipOpenNewFileInZip(zipfile, files[i], NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) != ZIP_OK) {
            free(read_buffer);
            zipClose(zipfile, "");
            return 3;
        }
        
        // Open a file to read the data.
        FILE *in = fopen(files[i], "rb");
        if (in == NULL) {
            free(read_buffer);
            zipCloseFileInZip(zipfile);
            zipClose(zipfile, "");
            return 4;
        }

        size_t size;
        do    
        {
            size = fread(read_buffer, 1, READ_SIZE, in);
            
            
            if(size<READ_SIZE) {
                if(!feof(in)) {
                    free(read_buffer);
                    zipCloseFileInZip(zipfile);
                    zipClose(zipfile, "");
                    fclose(in);
                    return 5;
                }
            }
            
            if(size>0) {
                //write data to zip
                if (zipWriteInFileInZip(zipfile, read_buffer, size) != ZIP_OK) {
                    //printf("error %d\n", error);
                    free(read_buffer);
                    zipCloseFileInZip(zipfile);
                    zipClose(zipfile, "");
                    fclose(in);
                    return 6;
                }
            }
        } while (size > 0);

        fclose(in);

        zipCloseFileInZip(zipfile);
    }
    
    free(read_buffer);
    zipClose(zipfile, comment);
    
    return 0;
}
