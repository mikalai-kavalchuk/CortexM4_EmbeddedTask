#ifndef INC_USER_FUNCTIONS_H_
#define INC_USER_FUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include "termcolor.h"

/**
 * @brief Typedef for user commands
 */
typedef struct
{
    bool (*run)(int);
    const char *command_name;
    const char *command_param;
} Command_t;


/**
  * @brief UserFunctiond Initialization
  * @retval true if initialized
  */
bool UserFunctions_Init(void);

/**
  * @brief Get count of user functions
  * @retval count of functions
  */
uint8_t UserFunctions_GetFuncCount(void);

/**
  * @brief Get pointer to the used defined function by item index
  * @param[in] item index of function
  * @retval pointer to the function
  */
Command_t* UserFunctions_GetFunc(uint8_t item);

#ifdef __cplusplus
}
#endif

#endif /* INC_USER_FUNCTIONS_H_ */
