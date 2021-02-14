/*!
    \file       memgrind_c.h
    \brief      Header file for memgrind_c
    
    \author     Gemuele Aludino
    \date       13 Feb 2021
 */

#ifndef MEMGRIND_C_H
#define MEMGRIND_C_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// Unit conversion functions from ns (s/ms/mcs)
double convert_ns_to_s(double nanoseconds);
double convert_ns_to_ms(double nanoseconds);
double convert_ns_to_mcs(double nanoseconds);

// Calculate elapsed time
double elapsed_time_ns(struct timespec *before, 
                       struct timespec *after);

double elapsed_time_ms(struct timespec *before, 
                       struct timespec *after);

double elapsed_time_s(struct timespec *before, 
                             struct timespec *after);

// Random number generator, given an open/close range [minimum, maximum)
int randrnge(int minimum, int maximum);

// Randomly generate true or false
bool randbool();

// Random string generator, of size length, into buffer
char *randstr(char *buffer, size_t length);

int cstr_compare(const void *c0, const void *c1);

#define MCS "µs"

#define RANDSTR__CHARS                                                         \
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!;"

void header_fputs(FILE *dest, const char *filename, const char *funcname, size_t lineno);

#ifdef CGCS_MALLOC_ENABLE_LOGGING
#define listlog() header_fputs(stdout, __FILE__, __func__, __LINE__)
#else
#define listlog()
#endif

/*!
    \brief
    
    \param[in]  nanoseconds

    \return
 */
inline double convert_ns_to_s(double nanoseconds) {
    return nanoseconds * pow(10.0, -9.0);
}

/*!
    \brief
    
    \param[in]  nanoseconds

    \return
 */
inline double convert_ns_to_ms(double nanoseconds) {
    return nanoseconds * pow(10.0, -6.0);
}


/*!
    \brief
    
    \param[in]  nanoseconds

    \return
 */
inline double convert_ns_to_mcs(double nanoseconds) {
    return nanoseconds * pow(10.0, -3.0);
}

/*!
    \brief
    
    \param[in]  before
    \param[in]  after

    \return
 */
inline double elapsed_time_ns(struct timespec *before, 
                                     struct timespec *after) {
   return ((pow(10.0, 9.0) * after->tv_sec) + after->tv_nsec)
   - ((pow(10.0, 9.0) * before->tv_sec) + before->tv_nsec);
}

/*!
    \brief
    
    \param[in]  before
    \param[in]  after

    \return
 */
inline double elapsed_time_ms(struct timespec *before, 
                                     struct timespec *after) {
    return elapsed_time_ns(before, after) * pow(10.0, -6.0);
}

/*!
    \brief
    
    \param[in]  before
    \param[in]  after

    \return
 */
inline double elapsed_time_s(struct timespec *before, 
                                    struct timespec *after) {
    return elapsed_time_ns(before, after) * pow(10.0, -9.0);
}

/*!
    \brief
    
    \param[in]  minimum
    \param[in]  maximum

    \return
 */
inline int randrnge(int minimum, int maximum) {
    return (rand() % (maximum - minimum)) + minimum;
}

/*!
    \brief

    \return
 */
inline bool randbool() {
    return (rand() % ((true + 1) - false)) + false;
}

inline int cstr_compare(const void *c0, const void *c1) {
    return strcmp(*(char **)(c0), *(char **)(c1));
}

#endif /* MEMGRIND_C_H */
