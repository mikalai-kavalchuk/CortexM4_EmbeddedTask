#ifndef INC_USER_FUNCTIONS_H_
#define INC_USER_FUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "termcolor.h"

#define COMMANDS_COUNT          4

/**
 * Typdef for user commands
 */
typedef struct
{
    const char *command_name;
    int (*run_func)(int);
} Command_t;

extern Command_t commands_list[COMMANDS_COUNT];




#ifdef __cplusplus
}
#endif

#endif /* INC_USER_FUNCTIONS_H_ */
