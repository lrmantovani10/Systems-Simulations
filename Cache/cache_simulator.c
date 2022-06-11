#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef long long el_size;

// Get number of sets in cache
int get_sets(int s){
    int set_num = 1;
    while(s > 0){
        set_num *= 2;
        s--;
    }

    return set_num;
}

// Function to process data in each line of the trace files
void processing_data(char operation, el_size memory, int s, int e, int b, 
        el_size **cache, int *hits, int *misses, int *evictions, int verbose){

    int upper_limit = (8 * sizeof(el_size)) - 1;
    int n_tag = (upper_limit + 1) - (s + b);
    el_size one = 1;
    
    el_size set_index = (memory >> b) & ~((one << upper_limit) >> (upper_limit - s));

    el_size tag = (memory >> (s + b)) & ~((
        one << upper_limit) >> (upper_limit - n_tag));

    int partial_hits = 0;
    int partial_misses = 0;
    int partial_evictions = 0;

    // Hits
    for (int i = 0; i < e; i++){
        if(cache[set_index][i] == ((tag << 1) + 1)){

            partial_hits++;

            // Make most recently used tag be the first in the set array
            for(int t = i; t > 0; t--)
                cache[set_index][t] = cache[set_index][t - 1];

            cache[set_index][0] = (tag << 1) + 1;

            break;
        }
    }
    
    // Misses
    if(!partial_hits){
        partial_misses++;

        int acc = 0;
        for (int a = 0; a < e; a++)
            acc += (cache[set_index][a] & 1);

        // Evictions occurr when there is no block with a starting 0 
        if(acc >= e)
            partial_evictions++;
        
        // LRU policy
        // Make most recently used tag be the first in the set array
        // Make the inserted tag the first, since it is the most recently used
        
        for(int y = e - 1; y > 0; y--)
            cache[set_index][y] = cache[set_index][y - 1];

        cache[set_index][0] = (tag << 1) + 1;

    }

    if(operation == 'M')
        partial_hits++;

    if(verbose)
        printf("\nHITS %d MISSES %d  EVICTIONS %d OPERATION %c", 
            partial_hits, partial_misses, partial_evictions, operation);

    *hits += partial_hits;
    *misses += partial_misses;
    *evictions += partial_evictions;

    return;
}

// Function to obtain the numerical value of a string of numbers
el_size process_number(char *inp, int change){
    int output;
    if (change == 1)
        output = (int) strtol(inp, NULL, 16);
    else
        output = atoi(inp);
    return ((el_size) output);
}

int main(int argc, char *argv[])
{   
    // Determine user parameters
    int s, e, b;
    int verbose = 0;
    char *filename;

    for(short i = 0; i < argc; i++){
        if(strcmp(argv[i],"-s") == 0){
            s = atoi(argv[i + 1]);
        }

        else if (strcmp(argv[i],"-E") == 0)
            e = atoi(argv[i + 1]);

        else if (strcmp(argv[i],"-b") == 0)
            b = atoi(argv[i + 1]);

        else if (strcmp(argv[i],"-t") == 0)
            filename = argv[i + 1];
        
        else if (strcmp(argv[i],"-v") == 0)
            verbose = 1;
    }

    // Build Cache
    // Obtain number of sets from s
    int set_num = get_sets(s);

    el_size **cache = malloc(sizeof(el_size *) * set_num);

    // Populate blocks with zeros
    for(int i = 0; i < set_num; i++){
        cache[i] = malloc(sizeof(el_size) * e);
        for(int a = 0; a < e; a++)
            cache[i][a] = 0;
    }

    // Open file
    FILE *f = fopen(filename, "r");
    printf("%d %d %d %s", s, e, b, filename);
    if (!f){
        printf("File %s couldn't be opened! ", filename);
        return 1;
    }

    // Control flow variables
    char curr_char = 0;
    int curr_el = 0;
    int ignore_line = 0;

    // Parameters defined in each line
    char operation;
    int address_length = 17;
    char *accumulated_address = (char *) malloc(17);
    strcpy(accumulated_address, " ");
    accumulated_address[address_length - 1] = '\0';

    el_size memory; 

    int hits = 0;
    int misses = 0;
    int evictions = 0;

    // Read trace file lines 
    while(curr_char != EOF){
        curr_char = fgetc(f);
        if (curr_char != 10){
            if (!ignore_line){

                if(curr_el == 0 && (unsigned char) curr_char != 32)
                    ignore_line = 1;

                else if(curr_el == 1)
                    operation = curr_char;

                else if (curr_el >= 3 && curr_char != 44){
                    char curr_string[2] = {curr_char, '\0'};
                    strcat(accumulated_address, curr_string);
                }

                else if (curr_char == 44){
                    memory = process_number(accumulated_address, 1);
                    strcpy(accumulated_address, "");
                }

                curr_el += 1;
            }
        }
    
        else{

            strcpy(accumulated_address, "");
            if (!ignore_line){
                processing_data(operation, memory, s, e, b, cache, 
                &hits, &misses, &evictions, verbose);
            }

            curr_el = 0;
            ignore_line = 0;
        }

    }

    fclose(f);
    printSummary(hits, misses, evictions);
    free(accumulated_address);

    // Freeing cache array and its elements from the heap
    for(int b = 0; b < set_num; b++)
        free(cache[b]);

    free(cache);

    return 0;
}