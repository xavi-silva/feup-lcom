#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>
#include "game.h"

int main(int argc, char* argv[]) {
    lcf_set_language("EN-US");
    lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");
    lcf_log_output("/home/lcom/labs/proj/src/output.txt");
    if (lcf_start(argc, argv)) return 1;
    lcf_cleanup();
    return 0;
}

int (proj_main_loop)(int argc, char* argv[]) {
    if(game_init() != 0){
        printf("Error initializing the game\n");
        return 1;
    }
    if(game_main_loop() != 0){
        printf("Error running the game\n");
        return 1;
    }
    if(game_end() != 0){
        printf("Error exiting the game\n");
        return 1;
    }
    return 0;
}
