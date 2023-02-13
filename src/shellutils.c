#include "../include/shellutils.h"
#include "../include/logs.h"
#include "../include/bootutils.h"
#include "../include/ErrorCodes.h"
#include "../include/display.h"

#define DIRECTORY_DELIM ('\\')
#define DIRECTORY_DELIM_STR ("\\")
#define CURRENT_DIR (".")
#define PREVIOUS_DIR ("..")


/*
* This function concatenates two path sides
* lhs = "/usr/bin" + rhs = "/lib/lol" = /usr/bin/lib/lol
* lhs - left hand side, rhs - right hand side
* func caller has to free the char_t* buffer
*/
char_t* ConcatPaths(const char_t* lhs, const char_t* rhs)
{
    size_t lhsLen = strlen(lhs);
    size_t rhsLen = strlen(rhs);

    // null terminator + additinal '/' between the rhs and lhs
    char_t* newPath = malloc(lhsLen + rhsLen + 2);
    if(newPath == NULL)
    {
        Log(LL_ERROR, 0, "Failed to allocate memory for two path concatenation");
        return NULL;
    }
    strncpy(newPath, lhs, lhsLen); // copy the left path to newly allocated bufer

    //before adding the right hand side, we have to make sure that only one side has has '/'
    // if the last char in lhs is not '/' and the first char in rhs is not '/', we have to append it
    size_t lhsLastIndex = strlen(lhs) -1;
    if(lhs[lhsLastIndex] != '\\' && rhs[0] != '\\')
    {
        strcpy(newPath + lhsLen, '\\', 1);
    }
    // Avoid duplicating '\\' in newPath
    if(lhs[lhsLastIndex] == rhs[0] == '\\')
    {
        rhs++; // advance pointer to next character
    }
    strncat(newPath + lhsLen, rhs, rhsLen);

    return newPath; // note the newPath wasnt freed, the once calling the function has to free it

}