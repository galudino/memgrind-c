/*!
    \file       memgrind_c.h
    \brief      Header file for memgrind_c
    
    \author     Gemuele Aludino
    \date       13 Feb 2021
 */

#ifndef MEMGRIND_C_H
#define MEMGRIND_C_H

#include <stdio.h>

void header_fputs(FILE *dest, const char *filename, const char *funcname, size_t lineno);

#endif /* MEMGRIND_C_H */
