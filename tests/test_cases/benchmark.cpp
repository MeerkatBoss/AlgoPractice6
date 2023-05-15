#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>

#include "meerkat_assert/asserts.h"

#include "test_utils/math.h"
#include "test_utils/display.h"

#include "benchmark.h"

#ifdef OPEN_ADDR
#define TEST_NAME "open_addr_hash_table"
#else
#define TEST_NAME "closed_addr_hash_table"
#endif

struct ExecTime
{
    double user_ms;
    double sys_ms;
};


static int fill_data(size_t start_size, size_t end_size, size_t step_size,
                     double* sys_time, double* user_time);

static int get_execution_time(int argc, char** argv, ExecTime* exec_time);

static void run_child(int argc, char** argv);


int run_test_benchmark([[maybe_unused]] int argc,
                       [[maybe_unused]] const char* const* argv,
                       const TestConfig* config)
{
    const size_t start_size = 10'000;
    const size_t end_size   = 1'000'000;
    const size_t step_size  = 10'000;
    const size_t repeat_count = (end_size - start_size) / step_size + 1;

    FILE *output = NULL;
    double *sys_time = NULL, *user_time = NULL;

    SAFE_BLOCK_START
    {
        if (config->filename)
        {
            ASSERT_MESSAGE(
                output = fopen(config->filename,
                                config->append_to_file ? "a" : "w"),
                action_result != NULL,
                "Failed to open output file");
        }
        else output = stdout;

        ASSERT_MESSAGE(
            sys_time = (double*) calloc(repeat_count, sizeof(*sys_time)),
            action_result != NULL,
            "Failed to allocate memory");
        ASSERT_MESSAGE(
            user_time = (double*) calloc(repeat_count, sizeof(*user_time)),
            action_result != NULL,
            "Failed to allocate memory");

        ASSERT_ZERO_MESSAGE(
            fill_data(start_size, end_size, step_size,
                      sys_time, user_time),
            "Failed to run benchmark");
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        fprintf(stderr, "Error: %s\n", assertion_info.message);
        return 1;
    }
    SAFE_BLOCK_END

    for (size_t iter = 0, i = start_size; i <= end_size; ++iter, i += step_size)
        fprintf(output, TEST_NAME",%zu,%.3lf\n", i, sys_time[iter]
                                                    + user_time[iter]);

    putchar('\n');
    free(sys_time);
    free(user_time);

    return 0;
}

static int fill_data(size_t start_size, size_t end_size, size_t step_size,
                     double* sys_time, double* user_time)
{
    const size_t data_size = (end_size - start_size) / step_size + 1;

    double last_ms = NAN;
    const size_t repeat = 5;
    char test_size[32] = "";

    char argc = 2;
    const char* argv[] = { "./build/bin/hash_practice",
                           test_size,
                           NULL };

    for (size_t i = start_size, iter = 0; i <= end_size; ++iter, i += step_size)
    {
        double sum_sys = 0, sum_user = 0;
        for (size_t j = 0; j < repeat; j++)
        {
            sprintf(test_size, "%zu", i);
            progress_bar(iter*repeat + j, data_size * repeat, last_ms);

            ExecTime exec_time = {};
            if (get_execution_time(
                        argc, const_cast<char**>(argv), &exec_time) < 0)
                return -1;
            
            sum_sys  += exec_time.sys_ms;
            sum_user += exec_time.user_ms;

            last_ms = exec_time.sys_ms + exec_time.user_ms;
        }

        sys_time[iter]  = sum_sys  / repeat;
        user_time[iter] = sum_user / repeat;
    }

    progress_bar(data_size * repeat, data_size*repeat, NAN);

    return 0;
}

__always_inline
static double get_utime(rusage* usage)
{
    return (double) usage->ru_utime.tv_sec * 1000.0
         + (double) usage->ru_utime.tv_usec / 1000.0;
}

__always_inline
static double get_stime(rusage* usage)
{
    return (double) usage->ru_stime.tv_sec * 1000.0
         + (double) usage->ru_stime.tv_usec / 1000.0;
}

static int get_execution_time([[maybe_unused]]int argc, char** argv,
                                              ExecTime* exec_time)
{
    pid_t child = fork();

    if (child < 0)
    {
        perror("Failed to spawn proccess");
        return -1;
    }

    if (child == 0)
        run_child(argc, argv);

    rusage usage = {};
    double sum_sys = 0, sum_user = 0;

    getrusage(RUSAGE_CHILDREN, &usage);
    sum_user = get_utime(&usage);
    sum_sys  = get_stime(&usage);
    
    int status = 0;
    if (waitpid(child, &status, 0) < 0)
    {
        perror("Failed to wait for child");
        return -1;
    }

    getrusage(RUSAGE_CHILDREN, &usage);

    exec_time->user_ms = get_utime(&usage) - sum_user;
    exec_time->sys_ms  = get_stime(&usage) - sum_sys;

    return 0;
}

__attribute__((noreturn))
static void run_child([[maybe_unused]]int argc, char** argv)
{
    int dev_null = open("/dev/null", O_WRONLY);

    if (dev_null < 0)   // On Windows
        dev_null = open("nul", O_WRONLY);

    // Redirect output to /dev/null or nul
    dup2(dev_null, STDOUT_FILENO);

    if (execvp(argv[0], argv) < 0)
    {
        perror("Failed to start program");
        exit(1);
    }

    /* Unreachable */
    exit(0);
}
