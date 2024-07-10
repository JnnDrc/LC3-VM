//----------------------------------------------------------
// Hex Compiler
// this program compiles ascii string hex numbers 
// to a binary file.
// OBS : the max hex value that can be written is FFFF.
//       change the type of 'hexValue' and re-compile
//       to support greater values.
//----------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

size_t compileHex(const char *inputFilePath, const char *outputFilePath) {
    FILE *in,*out;
    char line[MAX_LINE_LENGTH];
    size_t written = 0;

    in = fopen(inputFilePath, "r");
    if(in == NULL){
        fprintf(stderr,"ERR: Can't open %s",inputFilePath);
        exit(1);
    }
    out = fopen(outputFilePath, "wb");
    if(out == NULL){
        fprintf(stderr,"ERR: Can't open %s",outputFilePath);
        exit(1);
    }

    while (fgets(line, sizeof(line), in)) {
        size_t len = strlen(line);
        if(line[0] == '#') continue;
        strcpy(line,strtok(line,"#"));

        if(line[len - 1] == '\n') line[len - 1] = '\0';

        unsigned short hexValue;
        sscanf(line, "%x", &hexValue);

        written += fwrite(&hexValue, sizeof(hexValue), 1, out);
    }

    fclose(in);
    fclose(out);

    return written;
}

int main(int argc, char** argv) {
    char inPath[256], outPath[256];
    size_t written;
    if (argc < 2)
    {
        fprintf(stderr,"ERR: Missing arguments\n");
        fprintf(stderr,"EXPECTED: hexc input_file_path (OPTIONAL)output_file_path");
        return 1;
    }

    strcpy(inPath,argv[1]);
    if (argc < 3) strcpy(outPath,strcat(strtok(argv[1],"."),".o"));
    else strcpy(outPath,argv[2]);
    
    
    written = compileHex(inPath, outPath);

    fprintf(stdout,"Hex compiled\n");
    fprintf(stdout,"Wrote %llu bytes to %s\n",2*(written),outPath);
    return 0;
}