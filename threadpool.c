#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include <stdbool.h>
#include <unistd.h>
#include "threadpool.h"

struct tpool;
// typedef struct tpool tpool_t;

typedef void (*thread_func_t)(tpool_t, void *arg);

struct tpool_task
{
  thread_func_t func;
  void *arg;
  struct tpool_task *next;
};
typedef struct tpool_task tpool_task_t;

static tpool_task_t *create_task(thread_func_t func, void *arg);
static tpool_task_t *get_task(tpool_t pool);
void tpool_wait(tpool_t pool);
void tpool_destroy(tpool_t pool);

struct tpool
{

  /* TO BE COMPLETED BY THE STUDENT */
  tpool_task_t *head;
  tpool_task_t *tail;
  pthread_mutex_t mx;
  pthread_cond_t task_available;
  pthread_cond_t no_tasks_left;
  size_t working_cnt;
  size_t thread_cnt;
  bool stopped;
  pthread_t *threads;
};

// HELPERS

static tpool_task_t *create_task(thread_func_t func, void *arg)
{
  tpool_task_t *task;

  if (func == NULL)
    return NULL;

  task = malloc(sizeof(*task));
  task->func = func;
  task->arg = arg;
  task->next = NULL;
  return task;
}

static tpool_task_t *get_task(tpool_t pool)
{
  tpool_task_t *task;

  if (pool == NULL)
    return NULL;

  task = pool->head;
  if (task == NULL)
    return NULL;

  if (task->next == NULL)
  {
    pool->head = NULL;
    pool->tail = NULL;
  }
  else
  {
    pool->head = task->next;
  }

  return task;
}

/* Function executed by each pool worker thread. This function is
 * responsible for running individual tasks. The function continues
 * running as long as either the pool is not yet joined, or there are
 * unstarted tasks to run. If there are no tasks to run, and the pool
 * has not yet been joined, the worker thread must be blocked.
 *
 * Parameter: param: The pool associated to the thread.
 * Returns: nothing.
 */
void *run_tasks(void *param)
{
  tpool_t pool = param;

  /* TO BE COMPLETED BY THE STUDENT */
  tpool_task_t *task;

  while (1)
  {
    pthread_mutex_lock(&(pool->mx));

    // printf("pool working count: %zu\n\n", pool->working_cnt);
    while (pool->head == NULL && !pool->stopped)
    {
      // printf("pool stopped here\n");
      pthread_cond_wait(&(pool->task_available), &(pool->mx));
    }

    if (pool->stopped && pool->head == NULL && pool->working_cnt == 0)
      break;

    task = get_task(pool);
    pool->working_cnt++;
    pthread_mutex_unlock(&(pool->mx));

    if (task != NULL)
    {
      task->func(pool, task->arg);
    }

    pthread_mutex_lock(&(pool->mx));
    pool->working_cnt--;
    if (!pool->stopped && pool->working_cnt == 0 && pool->head == NULL)
      pthread_cond_signal(&(pool->no_tasks_left));
    pthread_mutex_unlock(&(pool->mx));
  }

  pool->thread_cnt--;
  // printf("here!\n");
  pthread_cond_signal(&(pool->no_tasks_left));
  pthread_mutex_unlock(&(pool->mx));

  return NULL;
}

/* Creates (allocates) and initializes a new thread pool. Also creates
 * `num_threads` worker threads associated to the pool, so that
 * `num_threads` tasks can run in parallel at any given time.
 *
 * Parameter: num_threads: Number of worker threads to be created.
 * Returns: a pointer to the new thread pool object.
 */
tpool_t tpool_create(unsigned int num_threads)
{

  /* TO BE COMPLETED BY THE STUDENT */

  tpool_t pool;
  // pthread_t thread;
  size_t i;

  pool = calloc(1, sizeof(*pool));
  pool->threads = calloc(1, num_threads * sizeof(pthread_t));
  pool->thread_cnt = num_threads;

  pthread_mutex_init(&(pool->mx), NULL);
  pthread_cond_init(&(pool->task_available), NULL);
  pthread_cond_init(&(pool->no_tasks_left), NULL);

  pool->head = NULL;
  pool->tail = NULL;
  pool->stopped = false;

  for (i = 0; i < num_threads; i++)
  {
    pthread_create(&pool->threads[i], NULL, run_tasks, pool);
    // pthread_detach(pool->threads[i]);
  }

  return pool;
}

/* Schedules a new task, to be executed by one of the worker threads
 * associated to the pool. The task is represented by function `fun`,
 * which receives the pool and a generic pointer as parameters. If any
 * of the worker threads is available, `fun` is started immediately by
 * one of the worker threads. If all of the worker threads are busy,
 * `fun` is scheduled to be executed when a worker thread becomes
 * available. Tasks are retrieved by individual worker threads in the
 * order in which they are scheduled, though due to the nature of
 * concurrency they may not start exactly in the same order. This
 * function returns immediately, and does not wait for `fun` to
 * complete.
 *
 * Parameters: pool: the pool that is expected to run the task.
 *             fun: the function that should be executed.
 *             arg: the argument to be passed to fun.
 */
void tpool_schedule_task(tpool_t pool, void (*fun)(tpool_t, void *), void *arg)
{

  /* TO BE COMPLETED BY THE STUDENT */

  tpool_task_t *task;

  if (pool == NULL)
  {
    return;
  }

  task = create_task(fun, arg);
  if (task == NULL)
  {
    return;
  }

  pthread_mutex_lock(&(pool->mx));
  if (pool->head == NULL)
  {
    pool->head = task;
    pool->tail = pool->head;
  }
  else
  {
    pool->tail->next = task;
    pool->tail = task;
  }

  pthread_cond_broadcast(&(pool->task_available));
  pthread_mutex_unlock(&(pool->mx));
}

/* Blocks until the thread pool has no more scheduled tasks; then,
 * joins all worker threads, and frees the pool and all related
 * resources. Once this function returns, no additional tasks can be
 * scheduled, and the thread pool resources are destroyed/freed.
 *
 * Parameters: pool: the pool to be joined.
 */
void tpool_join(tpool_t pool)
{

  /* TO BE COMPLETED BY THE STUDENT */
  if (pool == NULL)
  {
    return;
  }

  /************************************************************************************************************/

  pthread_mutex_lock(&(pool->mx));
  while (1)
  {
    if ((!pool->stopped && pool->working_cnt != 0) || (pool->stopped && pool->thread_cnt != 0))
    {
      pthread_cond_wait(&(pool->no_tasks_left), &(pool->mx));
    }
    else
    {
      pool->stopped = true;
      break;
    }
  }
  pthread_cond_broadcast(&(pool->task_available));
  pthread_mutex_unlock(&(pool->mx));

  /************************************************************************************************************/

  // tpool_wait HERE (AGAIN)
  /************************************************************************************************************/

  pthread_mutex_lock(&(pool->mx));
  while (1)
  {
    if ((!pool->stopped && pool->working_cnt != 0) || (pool->stopped && pool->thread_cnt != 0))
    {
      pthread_cond_wait(&(pool->no_tasks_left), &(pool->mx));
    }
    else
    {
      pool->stopped = true;
      break;
    }
  }
  for (int i = 0; i < pool->thread_cnt; i++)
  {
    // printf("here\n");
    pthread_join(pool->threads[i], NULL);
  }
  pthread_mutex_unlock(&(pool->mx));

  /************************************************************************************************************/

  free((void *) pool->threads);
  free(pool);
}