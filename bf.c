#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 0

#define MAX_FILE_SIZE 1000000 // 1 MB
#define MEM_SIZE 100000 // 100 KB
#define MAX_LOOPS 100000

static char* array;
static char* ptr;

struct loop {
    unsigned begin;
    unsigned end;
};
static struct loop* loops;
static unsigned numLoops = 0;

void printHelp(const char* fn) {
    fprintf(stderr, "usage: %s <input>\n", fn);
    return;
}
void printLoops(void) {
    register unsigned i;
    puts("Loops:");
    for(i = 0; i < numLoops; ++i) {
        struct loop l = loops[i];
        printf("\t%u b:%u e:%u\n", i, l.begin, l.end);
    }
    return;
}

void jumpForward(unsigned* pos) {
    register unsigned i;
    for(i = 0; i < numLoops; ++i) {
        if(loops[i].begin == *pos) {
            *pos = loops[i].end;
            return;
        }
    }
}

void jumpBackward(unsigned* pos) {
    register unsigned i;
    for(i = 0; i < numLoops; ++i) {
        if(loops[i].end == *pos) {
            *pos = loops[i].begin;
            return;
        }
    }
}

int performOperation(const char* buffer, unsigned* pos) {
    switch(buffer[*pos]) {
        case '>':
            ptr++;
            break;
        case '<':
            ptr--;
            break;
        case '+':
            ++*ptr;
            break;
        case '-':
            --*ptr;
            break;
        case '.':
            putchar(*ptr);
            break;
        case ',':
            *ptr = getchar();
            break;
        case '[':
            if(*ptr != 0) break;
            jumpForward(pos);
            break;
        case ']':
            if(*ptr == 0) break;
            jumpBackward(pos);
            break;
        default:
            // fprintf(stderr, "Unrecognized token %c at pos %d\n", buffer[*pos], *pos);
            // return 1;
            break;
    }
    return 0;
}

int interpret(const char* filename) {
    FILE* file;
    file = fopen(filename, "r");
    if(!file) {
        fprintf(stderr, "failed to read file: %s\n", filename);
        return 1;
    }

    // reset data
    memset(array, 0, MEM_SIZE * sizeof(char));
    memset(loops, 0, MAX_LOOPS * sizeof(struct loop));
    ptr = array;

    char* buffer = (char*) malloc(sizeof(char) * MAX_FILE_SIZE);

    // read file into memory, skipping all unknown tokens
    char c;
    unsigned initpos = 0;
    int currentLoop = -1; // start counting at 0
    int builtLoops = 0;
    while((c = getc(file)) != EOF) {
        switch(c) {
            // create loop jump lookup array
            case '[':
                currentLoop++;
                loops[currentLoop].begin = initpos;
                loops[currentLoop].end   = MAX_FILE_SIZE;
                goto set;
            case ']':
                for(signed i = currentLoop; i >= 0; --i) {
                    // set end point for last unset loop
                    if(loops[i].end == MAX_FILE_SIZE) {
                        loops[i].end = initpos;
                        break;
                    }
                }
                builtLoops++;
            case '>':
            case '<':
            case '+':
            case '-':
            case '.':
            case ',':
set:
                buffer[initpos++] = c;
                break;
            default:
                // fprintf(stderr, "Unrecognized token %c at pos %d\n", buffer[*pos], *pos);
                // return 1;
                break;
        }
    }
    if(currentLoop - builtLoops != -1) {
        puts("Unbalanced loops detected");
        free(buffer);
        return 1;
    }
    numLoops = builtLoops;
    if(DEBUG) {
        printLoops();
        puts(buffer);
    }

    // interpret
    unsigned pos = 0;
    while(pos < initpos) {
        // printf("%d %c\n", pos, buffer[pos]);
        if(performOperation(buffer, &pos)) {
            break;
        }
        ++pos;
    }

    free(buffer);
    fclose(file);
    return 0;
}

int main(int argc, char** argv) {

    if(argc == 1) {
        printHelp(argv[0]);
        return 1;
    }

    array = (char*) malloc(sizeof(char) * MEM_SIZE);
    loops = (struct loop*) malloc(sizeof(struct loop) * MAX_LOOPS);

    register int i;
    for(i = 1; i < argc; ++i) {
        if(interpret(argv[i]))
            return 1;
    }
    return 0;
}
