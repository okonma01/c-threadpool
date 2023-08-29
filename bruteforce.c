#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <crypt.h>

#include "bruteforce.h"
#include "threadpool.h"

struct crypt_data data;

void my_task(tpool_t pool, void *arg)
{
    char **args = arg;
    char *encrypted_hash = malloc(sizeof(args[0]));
    char *str = malloc(sizeof(args[1]));

    strcpy(encrypted_hash, args[0]);
    strcpy(str, args[1]);
    char *str_hash = crypt_r(str, encrypted_hash, &data);

    // printf("args[0]: %s\n", args[0]);
    // printf("args[1]: %s\n", args[1]);
    // printf("str: %s\n", str);
    // printf("str_hash: %s\n", str_hash);
    if (strcmp(encrypted_hash, str_hash) == 0)
    {
        printf("%s\n", str);
        exit(0);
    }
    else
    {
        if (strlen(str) < 8)
        {
            // char *s = malloc(sizeof(char *));
            for (int i = 0; i <= 9; i++)
            {
                char **newargs = malloc(2*sizeof(char *));
                char *temp = malloc(10);
                // char ch = i + '0';
                sprintf(temp, "%s%d", str, i);
                
                // printf("str: %s\n", str);
                
                newargs[0] = encrypted_hash;
                newargs[1] = temp;

                tpool_schedule_task(pool, my_task, (void *)newargs);
                //strcpy(temp, str);

            }
        }
    }
}

void bruteforce(unsigned int num_threads, const char *encrypted_hash)
{
    tpool_t pool;
    pool = tpool_create(num_threads);
    // char *hash = crypt(encrypted_hash, "Ae");

    // printf("%s\n", encrypted_hash);
    // char *hash = encrypted_hash;
    char *args[2];
    args[0] = strdup(encrypted_hash);
    args[1] = "";
    tpool_schedule_task(pool, my_task, (void *)args);
    tpool_join(pool);
}