#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "circular_buffer.h"
#include "protected_buffer.h"
#include "utils.h"


// Initialise the protected buffer structure above. 
protected_buffer_t * cond_protected_buffer_init(int length) {
  protected_buffer_t * b;
  b = (protected_buffer_t *)malloc(sizeof(protected_buffer_t));
  b->buffer = circular_buffer_init(length);
  // Initialize the synchronization components
  pthread_mutex_t mutex;
  pthread_cond_t fullslot, emptyslot;
  pthread_cond_init(&fullslot, NULL);
  pthread_cond_init(&emptyslot, NULL);
  pthread_mutex_init(&mutex, NULL);
  return b;
}

// Extract an element from buffer. If the attempted operation is
// not possible immedidately, the method call blocks until it is.
void * cond_protected_buffer_get(protected_buffer_t * b){
  void * d;
  
  // Enter mutual exclusion
  pthread_mutex_lock(&b->mutex);
  // Wait until there is a full slot to get data from the unprotected
  // circular buffer (circular_buffer_get).
  if(circular_buffer_size(b->buffer)==0){
    pthread_cond_wait(&b->fullslot,&b->mutex);
  }
  d = circular_buffer_get(b->buffer);
  print_task_activity ("get", d);
  
  // Signal or broadcast that an empty slot is available in the
  // unprotected circular buffer (if needed)
  if(circular_buffer_size(b->buffer) == b->buffer->max_size-1){
    pthread_cond_signal(&b->emptyslot);
  }

  // Leave mutual exclusion
  pthread_mutex_unlock(&b->mutex);

  return d;
}

// Insert an element into buffer. If the attempted operation is
// not possible immedidately, the method call blocks until it is.
void cond_protected_buffer_put(protected_buffer_t * b, void * d){

  // Enter mutual exclusion
  pthread_mutex_lock(&b->mutex);
  // Wait until there is an empty slot to put data in the unprotected
  // circular buffer (circular_buffer_put).
  if(circular_buffer_size(b->buffer) == b->buffer->max_size){
    pthread_cond_wait(&b->emptyslot,&b->mutex);
  }
  circular_buffer_put(b->buffer, d);
  print_task_activity ("put", d);

  // Signal or broadcast that a full slot is available in the
  // unprotected circular buffer (if needed)
  if(circular_buffer_size(b->buffer)==1){
    pthread_cond_signal(&b->fullslot);
  }

  // Leave mutual exclusion
  pthread_mutex_unlock(&b->mutex);
}

// Extract an element from buffer. If the attempted operation is not
// possible immedidately, return NULL. Otherwise, return the element.
void * cond_protected_buffer_remove(protected_buffer_t * b){
  void * d;
  int    rc = 0;

  // Enter mutual exclusion
  rc = pthread_mutex_trylock(&b->mutex);
  if(rc != 0){
    return NULL;
  }

  // Signal or broadcast that an empty slot is available in the
  // unprotected circular buffer (if needed)
  if(circular_buffer_size(b->buffer) == b->buffer->max_size){
    pthread_cond_signal(&b->emptyslot);
  }

  d = circular_buffer_get(b->buffer);
  print_task_activity ("remove", d);
  
  // Leave mutual exclusion
  pthread_mutex_unlock(&b->mutex);

  return d;
}

// Insert an element into buffer. If the attempted operation is
// not possible immedidately, return 0. Otherwise, return 1.
int cond_protected_buffer_add(protected_buffer_t * b, void * d){
  int done;
  int rc=0;
  
  // Enter mutual exclusion
  rc = pthread_mutex_lock(&b->mutex);
  if(rc!=0){
    return 0;
  }

  // Signal or broadcast that a full slot is available in the
  // unprotected circular buffer (if needed)
  if(circular_buffer_size(b->buffer)==0){
    pthread_cond_signal(&b->fullslot);
  }

  done = circular_buffer_put(b->buffer, d);
  if (!done) d = NULL;
  print_task_activity ("add", d);

  // Leave mutual exclusion
  pthread_mutex_unlock(&b->mutex);
  return done;
}

// Extract an element from buffer. If the attempted operation is not
// possible immedidately, the method call blocks until it is, but
// waits no longer than the given timeout. Return the element if
// successful. Otherwise, return NULL.
void * cond_protected_buffer_poll(protected_buffer_t * b, struct timespec *abstime){
  void * d = NULL;
  int    rc = 0;
  
  // Enter mutual exclusion
  pthread_mutex_lock(&b->mutex);
  // Wait until there is an full slot to get data from in the unprotected
  // circular buffer (circular_buffer_put) but waits no longer than
  // the given timeout.
  if(circular_buffer_size(b->buffer)==0){
    rc = pthread_cond_timedwait(&b->fullslot,&b->mutex,abstime);
    if(rc!=0) return NULL;
  }
  // Signal or broadcast that a full slot is available in the
  // unprotected circular buffer (if needed)
  if(circular_buffer_size(b->buffer) == b->buffer->max_size){
    pthread_cond_signal(&b->emptyslot);
  }

  d = circular_buffer_get(b->buffer);
  print_task_activity ("poll", d);

  // Leave mutual exclusion
  pthread_mutex_unlock(&b->mutex);
  return d;
}

// Insert an element into buffer. If the attempted operation is not
// possible immedidately, the method call blocks until it is, but
// waits no longer than the given timeout. Return 0 if not
// successful. Otherwise, return 1.
int cond_protected_buffer_offer(protected_buffer_t * b, void * d, struct timespec * abstime){
  int rc = 0;
  int done = 0;
  
  // Enter mutual exclusion
  pthread_mutex_lock(&b->mutex);

  // Wait until there is an empty slot to put data in the unprotected
  // circular buffer (circular_buffer_put) but waits no longer than
  // the given timeout.
  if(circular_buffer_size(b->buffer) == b->buffer->max_size){
    rc = pthread_cond_timedwait(&b->emptyslot,&b->mutex, abstime);
    if(rc!=0) return 0;
  }

  // Signal or broadcast that a full slot is available in the
  // unprotected circular buffer (if needed)
  if(circular_buffer_size(b->buffer)==1){
    pthread_cond_signal(&b->fullslot);
  }
  
  done = circular_buffer_put(b->buffer, d);
  if (!done) d = NULL;
  print_task_activity ("offer", d);
    
  // Leave mutual exclusion
  pthread_mutex_unlock(&b->mutex);
  return done;
}
