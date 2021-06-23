/*!
    \file       memgrind_c.c
    \brief      Client source file for memgrind_c
    
    \author     Gemuele Aludino
    \date       13 Feb 2021

    \details
    memgrind unit tests
 
    A:  malloc() 1 byte and immediately free it - do this 150 times
 
    B:  malloc() 1 byte, store the pointer in an array - do this 150 times
        Once you've malloc()'ed 50 byte chunks, then free() the 50 1-byte
        pointers one-by-one.
 
    C:  Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer
        Do this until you have allocated 50 times.
 
        Keep track of each operation
        so that you eventually malloc 50 bytes in total.
  
        If you have already allocated 50 times,
        disregard the random and just free() on each iteration.
  
        Keep track of each operation
        so that you eventually free() all pointers.
        Don't allow a free() if you have no pointers to free().
  
    D:  Randomly choose between a randomly-sized malloc() or free()ing a
        pointer -- do this many times (see below)
  
        Keep track of each malloc 
        so that all mallocs do not exceed your total memory capacity.
  
        Keep track of each operation so that you eventually malloc 50 times.
        Keep track of each operation so that eventually free() all pointers
        Choose a random allocation size between 1 and 64 bytes.
  
    E:  A workload of your choosing.
    F:  A workload of your choosing.
  
        Describe both workloads in testplan.txt
  
    Your memgrind.c should run all the workloads, one after another, 100 times.
    It should record the run time for each workload and store it.
  
    When all 100 iterations of all the workloads have been run,
  
    memgrind.c should calculate the mean time for each workload to execute
    and output them in sequence.
  
    You might find the gettimeofday(struct timeval *tv, struct timezone *tz)
    function in the <time.h> library useful.
  
    You should run memgrind yourself and include its results in your readme.pdf.
    Be sure to discuss your findings, especially any interesting or unexpected
    results.
 */

#define _POSIX_C_SOURCE 199390L

//#define CGCS_MALLOC_ENABLE_LOGGING

#define MGR_ENABLE_TEST_A
#define MGR_ENABLE_TEST_B
#define MGR_ENABLE_TEST_C
#define MGR_ENABLE_TEST_D
#define MGR_ENABLE_TEST_E
#define MGR_ENABLE_TEST_F

#include "memgrind_c.h"

#include "cgcs_ulog.h"
#include "cgcs_vector.h"
#include "cgcs_malloc.h"

#include <stdarg.h>
#include <stdint.h>

typedef void (*memgrind_func_t)(uint32_t, uint32_t, uint32_t);

void mgr_run_test( memgrind_func_t test,
                   char tch,
                   uint32_t min,
                   uint32_t max,
                   uint32_t interval,
                   FILE *dest);

// memgrind: tests a through f (in order)
void mgr_simple_alloc_free(uint32_t max_iter, uint32_t alloc_sz, uint32_t unused_value);
void mgr_alloc_array_interval(uint32_t max_iter, uint32_t alloc_sz, uint32_t interval);
void mgr_alloc_array_range(uint32_t max_allocs, uint32_t alloc_sz_min, uint32_t alloc_sz_max);
void mgr_char_ptr_array(uint32_t min, uint32_t max, uint32_t unused_value);
void mgr_vector(uint32_t min, uint32_t max, uint32_t initial);

#define MGR_A_ITER_MAX 150

#define MGR_B_ITER_MAX 150
#define MGR_B_INTERVAL 50

#define MGR_C_ITER_MAX 50

#define MGR_D_ALLOC_MIN 1
#define MGR_D_ALLOC_MAX 64
#define MGR_D_ITER_MAX 50

#define MGR_E_MIN 29
#define MGR_E_MAX 59

#define MGR_F_MIN 8
#define MGR_F_MAX 32
#define MGR_F_INITIAL 5

#define MGR_MAX_ITER 100

/*!
    \brief      Program execution begins and ends here.
 
    \param[in]  argc    Command line argument count
    \param[in]  argv    Command line arguments
 
    \return     Exit status; 0 on success, nonzero on failure.
 */
int main(int argc, const char *argv[]) {
    /*
        Results may output to a filestream of choice, if preferred.
        Default is stdout.
     */
    FILE *stream = stdout;

    // Important for randomization.
    srand(time(NULL));

    fprintf(stream, "\n%s\n"
                    "%s %lu %s\n\n"
                    "%s (%s)\n\n"
                    "%s\n"
                    "%s\t\t%s\t\t%s\t\t%s\n"
                    "%s\n",
                    KGRN_b"cgcs memory allocator stress tests"KNRM,
                    "Each indvidual test is run", (long int)(MGR_MAX_ITER), "times and wall-clock time averaged.",
                    "All times are expressed in", MCS,
                    "-------------------------------------------------------------",
                    KWHT_b"test"KNRM, KWHT_b"mean"KNRM, KWHT_b"slowest"KNRM, KWHT_b"total"KNRM,
                    "-------------------------------------------------------------"
    );

    #ifdef MGR_ENABLE_TEST_A
    mgr_run_test(mgr_simple_alloc_free,        // test a 
                  'a',                         // alloc 1 byte, free 1 byte 
                  MGR_A_ITER_MAX,              // run 150 times 
                  1,                           // allocation size: 1 byte 
                  0,                           // (unused parameter) 
                  stream);                     // output to stdout 
    #endif

    #ifdef MGR_ENABLE_TEST_B
    mgr_run_test(mgr_alloc_array_interval,     // test b 
                  'b',                         // allocate to limit, then free 
                  MGR_B_ITER_MAX,              // run 150 times 
                  1,                           // allocation size: 1 byte 
                  MGR_B_INTERVAL,              // limit: 50 
                  stream);                     // output to stdout 
    #endif

    #ifdef MGR_ENABLE_TEST_C
    mgr_run_test(mgr_alloc_array_range,        // test c 
                  'c',                         // randomly alloc/free 1 byte 
                  MGR_C_ITER_MAX,              // run 50 times 
                  1,                           // allocation size min: 1 byte 
                  1,                           // allocation size max: 1 byte 
                  stream);                     // output to stdout 
    #endif

    #ifdef MGR_ENABLE_TEST_D
    mgr_run_test(mgr_alloc_array_range,        // test d 
                  'd',                         // randomly alloc/free 
                  MGR_D_ITER_MAX,              // run 50 times 
                  MGR_D_ALLOC_MIN,             // allocation size min: 1 byte 
                  MGR_D_ALLOC_MAX,             // allocation size max: 64 bytes 
                  stream);                     // output to stdout 
    #endif

    #ifdef MGR_ENABLE_TEST_E
    mgr_run_test(mgr_char_ptr_array,           // test e 
                  'e',                         // buffer of random-size buffers 
                  MGR_E_MIN,                   // min buffer size: 29 bytes 
                  MGR_E_MAX,                   // max buffer size: 59 bytes 
                  0,                           // (unused parameter) 
                  stream);                     // output to stdout 
    #endif

    #ifdef MGR_ENABLE_TEST_F
    mgr_run_test(mgr_vector,                   // test f 
                  'f',                         // "vector" of string test 
                  MGR_F_MIN,                   // min string size: 8 bytes 
                  MGR_F_MAX,                   // max string size: 32 bytes 
                  MGR_F_INITIAL,               // initial vector size: 5 elems 
                  stream);                     // output to stdout 
    #endif

    fprintf(stream, "\n");
    return EXIT_SUCCESS;
}

/*!
    \brief  function that conducts the stress test addressed by the
            callback function pointer test, and output results to dest
  
    \param[in]  test    pointer-to-function that represents a test case
    \param[in]  tch     character that will print to dest, represents test case
    \param[in]  min     a nonnegative minimum value (differs between test cases)
    \param[in]  max     a nonnegative maximum value (differs between test cases)
    \param[in]  interval nonnegative interval value (differs between test cases)
    \param[in]  dest    destination file stream
 */
void mgr_run_test( memgrind_func_t test,
                   char tch,
                   uint32_t min,
                   uint32_t max,
                   uint32_t interval,
                   FILE *dest) {
    struct timespec x = { 0.0, 0.0 };       // start time (secs, nsecs)
    struct timespec y = { 0.0, 0.0 };       // end time (secs, nsecs)

    double total_ns = 0.0;
    double elapsed_ns_avg = 0.0;
    double time_slowest = 0.0;
    double time_this = 0.0;

    for (uint32_t i = 0; i < 1; ++i) {
        clock_gettime(CLOCK_REALTIME, &x);  // start clock
        test(min, max, interval);           // run test
        clock_gettime(CLOCK_REALTIME, &y);  // stop clock
        
        time_this = elapsed_time_ns(&x, &y);
        time_slowest = time_this > time_slowest ? time_this : time_slowest;

        total_ns += time_this;
    }

    elapsed_ns_avg = total_ns / MGR_MAX_ITER;

    fprintf(dest,
            "%s%c%s\t\t%.5lf %s%s%s\t%.5lf %s%s%s\t%.5lf %s%s%s\t\t\n",
            KGRN_b,
            tch,
            KNRM,
            convert_ns_to_mcs(elapsed_ns_avg),
            KGRY,
            MCS,
            KNRM,
            convert_ns_to_mcs(time_slowest),
            KGRY,
            MCS,
            KNRM,
            convert_ns_to_mcs(total_ns),
            KGRY,
            MCS,
            KNRM);
}

/*!
    \brief  Test a: malloc() alloc_sz byte(s) 
            and immediately frees it, max_iter times
  
    \param[in]  max_iter    a nonnegative value, max iterations for test
    \param[in]  alloc_sz    a nonzero value, fixed allocation size
    \param[in]  unused_value unused value -- needed for function uniformity
 */
void mgr_simple_alloc_free(uint32_t max_iter, uint32_t alloc_sz, uint32_t unused_value) {
    listlog();

    for (uint32_t i = 0; i < max_iter; ++i) {
        char *ch = cgcs_malloc(alloc_sz);
        listlog();

        if (ch) { 
            cgcs_free(ch);
            listlog();
        }
    }

    listlog();
}

/*!
    \brief  Test b: malloc() alloc_sz byte(s), stores it in an array -
            After interval allocations, free interval pointers.
            Repeats until max_iter allocations have been reached
  
    \param[in]  max_iter    a nonnegative value, max iterations for test
    \param[in]  alloc_sz    a nonzero value, fixed allocation size
    \param[in]  unused_value unused value -- needed for function uniformity
 */
void mgr_alloc_array_interval(uint32_t max_iter, uint32_t alloc_sz, uint32_t interval) {
    listlog();

    // ptr to buffer of ptrs to char buffer
    // alloc memory for buffer of pointers and establish sentinel
    char **ch_ptrarr = cgcs_malloc(sizeof *ch_ptrarr * max_iter);
    char **sentinel = ch_ptrarr + interval;

    uint32_t i = 0;             // runs from [0, max_iter)
    
    while (i < max_iter) {
        char *ch = cgcs_malloc(alloc_sz);  // allocate memory for ptr to char buffer

        if (ch) {
            ch_ptrarr[i++] = ch;
        }

        // if at last pointer for this interval of allocations
        if ((ch_ptrarr + i) == sentinel) {
            listlog();

            uint32_t j = interval;  // runs from [interval, 0)

            while (j > 0) {
                /*
                    Retrieve the (i - j)th address from ch_ptrarr.
                    We allocate alloc_sz bytes interval times,
                    then do interval cgcs_free() calls for each 
                    alloc_sz byte allocations.
                */
                char **curr = ch_ptrarr + (i - j);

                if ((*curr)) {
                    // Free the pointer to char buffer.
                    cgcs_free((*curr));
                }

                --j;
            }

            /*
                Redefine sentinel for the next interval
                of allocations/deallocations.
            */
            sentinel = (ch_ptrarr + i) + interval;
            listlog();
        }
    }

    /*
        When we've done a total of max_iter allocations
        and max_iter deallocations, we free the
        pointer to the buffer of pointers to char buffers.
    */
    cgcs_free(ch_ptrarr);

    listlog();
}

/*!
    \brief  Test c/d: Randomly choose between 
            alloc_sz_min and alloc_sz_max byte(s)
            to allocate, repeats until max_allocs mallocs are made
  
    \param[in]  max_allocs      nonnegative value, max allocations for test
    \param[in]  alloc_sz_min    nonnegative value, minimum malloc size
    \param[in]  alloc_sz_max    nonnegative value, maximum malloc size
 */
void mgr_alloc_array_range(uint32_t max_allocs, uint32_t alloc_sz_min, uint32_t alloc_sz_max) {
    struct {
        uint32_t allocs;
        int to_free;
    } count = { 0, max_allocs };

    bool hit_max_allocs = false;

    char **ch_ptrarr = cgcs_malloc(sizeof *ch_ptrarr * max_allocs);
    memset(ch_ptrarr, 0, max_allocs);

    uint32_t k = 0;

    while (count.to_free > 0) {
        char **curr = NULL;

        bool nonnull = false;
        bool max_allocs_met = (count.allocs == max_allocs);

        uint32_t offset = 0;

        if (max_allocs_met) {
            if (hit_max_allocs == false) {
#ifdef CGCS_MALLOC_ENABLE_LOGGING
                LOG(__FILE__, KMAG_b "--- max allocs met ---");
#endif
                hit_max_allocs = true;
                listlog();
            }

            offset = count.to_free - 1;

            curr = ch_ptrarr + offset;
            nonnull = (*curr) != NULL;

            if (nonnull) {
                cgcs_free((*curr));
                (*curr) = NULL;

#ifdef CGCS_MALLOC_ENABLE_LOGGING
                LOG(__FILE__, KYEL_b "freed");
#endif
            } else {
#ifdef CGCS_MALLOC_ENABLE_LOGGING
                LOG(__FILE__, KYEL_b "skipped");
#endif
            }

            --count.to_free;
        } else {
            if (randbool()) {
#ifdef CGCS_MALLOC_ENABLE_LOGGING
                LOG(__FILE__, "will free");
#endif

                if (count.allocs > 0) {
                    offset = count.allocs - 1;

                    curr = ch_ptrarr + offset;
                    nonnull = (*curr) != NULL;

                    if (nonnull) {
                        cgcs_free((*curr));
                        (*curr) = NULL;

#ifdef CGCS_MALLOC_ENABLE_LOGGING
                        LOG(__FILE__, KYEL_b "freed");
#endif
                    } else {
#ifdef CGCS_MALLOC_ENABLE_LOGGING
                        LOG(__FILE__, KRED_b "nothing to free");
#endif
                    }
                } else {
#ifdef CGCS_MALLOC_ENABLE_LOGGING
                    LOG(__FILE__, KRED_b "nothing to free");
#endif
                }
            } else {
                char *ch = NULL;
                uint32_t size = 0;

                size = alloc_sz_min == alloc_sz_max ?
                           alloc_sz_min :
                           randrnge(alloc_sz_min, alloc_sz_max);

                ch = cgcs_malloc(size);

                if (ch) {
                    *(ch_ptrarr + count.allocs) = ch;
                    ++count.allocs;

#ifdef CGCS_MALLOC_ENABLE_LOGGING
                    LOG(__FILE__, "malloc()'ed");
#endif
                }
            }
        }

        ++k;
    }

    cgcs_free(ch_ptrarr);
    ch_ptrarr = NULL;

#ifdef CGCS_MALLOC_ENABLE_LOGGING
    printf("\ntotal iterations: %s%d%s\n\n", KYEL_b, k, KNRM);
#endif
    listlog();
}

/*!
    \brief      Test e:
                malloc an array of (char *) of size max, named ch_ptrarr.
   
    \details    for all (char *) in ch_ptrarr,
                    malloc an random size between 1 and max
  
                Then for all (char *) in ch_ptrarr,
                    randomly decide to free the current (char *), or keep it
                    (artificially creates fragmentation)
  
                Again, for all (char *) in ch_ptrarr,
                    malloc a random size between min and max
                    (the challenge is finding potentially bigger chunks
                     despite having freed blocks of sizes that may have
                     been smaller - tests coalescence)
  
                Finally, for all (char *) in ch_ptrarr,
                    free all (char *)
  
                Free ch_ptrarr.
 
    \param[in]  min     minimum byte value to allocate
    \param[in]  max     max iterations/max byte value to allocate
    \param[in]  unused_value unused value - needed for function uniformity
 */
void mgr_char_ptr_array(uint32_t min, uint32_t max, uint32_t unused_value) { 
    char **ch_ptrarr = cgcs_malloc(sizeof *ch_ptrarr * max);

    for (uint32_t i = 0; i < max; ++i) {
        ch_ptrarr[i] = cgcs_malloc(randrnge(1, max + 1));
    }

    for (uint32_t i = 0; i < max; ++i) {
        bool to_erase = randbool();

        if (to_erase) {
            if (ch_ptrarr[i]) {
                cgcs_free(ch_ptrarr[i]);
                ch_ptrarr[i] = NULL;
            }
        }
    }

#ifdef CGCS_MALLOC_ENABLE_LOGGING
    listlog();
#endif

    for (uint32_t i = 0; i < max; ++i) {
        if (ch_ptrarr[i] == NULL) {
            int num = randrnge(min, max + 1);
            ch_ptrarr[i] = cgcs_malloc(num);
        }
    }

#ifdef CGCS_MALLOC_ENABLE_LOGGING
    listlog();
#endif

    for (uint32_t i = 0; i < max; ++i) {
        if (ch_ptrarr[i]) {
            cgcs_free(ch_ptrarr[i]);
        }
    }

    cgcs_free(ch_ptrarr);

#ifdef CGCS_MALLOC_ENABLE_LOGGING
    listlog();
#endif
}

/*!
    \brief  test f:
            allocate a pointer to a vector ADT,
            initialize its internal buffer to size initial.
    
    \details    allocate memory for the vector structure pointer

                for i = [0...max + 1)
                    randomly generate a string of size between 1 and max
                    malloc memory for the string and push it to vector
                    (since the vector's buffer starts at size initial,
                    it will have to grow in order to accomodate the extra elements)
  
                    A new buffer will be malloced (double the size of the
                    current buffer) and the current buffer will be copied
                    to the newer, larger buffer. Then, the old buffer will be
                    freed and its pointer will be assigned the base address
                    of the new buffer.

                for i = [0, vsize(v))
                    Decide where the string at i should be kept, or freed.
  
                size = vsize(v)

                for i = [0, size)
                    randomly generate a string of size min and max
                    and push it to the vector

                free all strings in vector
                free the vector structure pointer
  
    \param[in]  min     minimum char count for string
    \param[in]  max     maximum char count for string, maximum strings added to vector
    \param[in]  initial initial starting size of vector's buffer
 */
void mgr_vector(uint32_t min, uint32_t max, uint32_t initial) {
    ///
    /// Begin allocation/construction of cgcs_vector
    ///   

    // Create an instance of cgcs_vector on the heap using cgcs_malloc,
    // and initialize its buffer with cgcs_malloc
    cgcs_vector *v = cgcs_vnew_allocfn(initial, cgcs_malloc);

#ifdef CGCS_MALLOC_ENABLE_LOGGING
    listlog();
#endif

    // Create a local array of char as temporary storage
    // for a randomized string
    char buffer[MGR_F_MAX];

    // for i = [0...max + 1), randomly generate a string of size [1, max],
    // store it in buffer, then duplicate buffer in a heap-allocated
    // instance, where ptr is that duplicate buffer's base address.
    // Push ptr to v.
    for (uint32_t i = 0; i < max; ++i) {
        int length = randrnge(1, max);
        char *str = randstr(buffer, length);

        char *ptr = cgcs_malloc(length + 1);
        strcpy(ptr, str);

        cgcs_vpushb_allocfreefn(v, &ptr, cgcs_malloc, cgcs_free);
    }

#ifdef CGCS_MALLOC_ENABLE_LOGGING
    listlog();
#endif

   // Iterate over each element in v and decide at random
   // whether to keep it or delete it.
   // Note that cgcs_vector_iterator is a (char **) --
   // it is the address of a (char *) within v's buffer.
   cgcs_vector_iterator it = cgcs_vbegin(v);
   
   while (it < cgcs_vend(v)) {
       if (randbool()) {
           // First, we must retrieve the (char *) in question
           // so we can release the memory at its address.
           cgcs_free(*it);

           // Then, we can "erase the slot" where that (char *)
           // once resided.
           // verase will move all (char *) from (it + 1) to (vend(v) - 1);
           // the (char *) at it will be overwritten.
           it = cgcs_verase(v, it);
       } else {
           ++it;
       }
   }

   // Add more randomized strings to v.
   // This time, each randomized string length can range from [min, max].
   size_t size = cgcs_vsize(v);

   for (size_t i = 0; i < size; ++i) {
       int length = randrnge(min, max);
       char *str = randstr(buffer, length);

       char *ptr = cgcs_malloc(length + 1);
       strcpy(ptr, str);

       cgcs_vpushb_allocfreefn(v, &ptr, cgcs_malloc, cgcs_free);
   }

   ///
   /// Begin destruction/delete of cgcs_vector.
   ///
   
   // Iterate from back to front, free each (char *) in v's buffer.
   for (it = cgcs_vend(v) - 1; it >= cgcs_vbegin(v); it--) {
       cgcs_free(*it);
   }

   // Destroy the heap-allocated instance of cgcs_vector's buffer,
   // then free the cgcs_vector instance itself
   cgcs_vdelete_freefn(v, cgcs_free);

#ifdef CGCS_MALLOC_ENABLE_LOGGING
    listlog();
#endif
}

/*!
    \brief      Randomly generate a string of size length
 
    \param[out] buffer  buffer to hold the characters for the string
    \param[in]  length  the desired length for the generated string
 */
char *randstr(char *buffer, size_t length) {
    const char *charset 
    = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!;";
    
    char *random_string = NULL;

    uint32_t key = 0;
    int n = 0;
    size_t string_length = 0;

    string_length = 26 * 2 + 10 + 7;
    random_string = buffer;

    if (random_string == NULL) {
        return NULL;
    }

    for (n = 0; n < length; n++) {
        key = rand() % string_length;
        random_string[n] = charset[key];
    }

    random_string[length] = '\0';
    return random_string;
}
