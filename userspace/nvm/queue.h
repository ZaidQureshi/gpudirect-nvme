#ifndef __NVME_QUEUE_H__
#define __NVME_QUEUE_H__
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __CUDACC__
#define __host__
#define __device__
#endif


#include "types.h"

struct completion;
struct command;


/* Enqueue a submission command
 * 
 * Enqueue a submission command in the specified SQ and return a pointer to
 * the queue slot in order to build the command inline in queue memory.
 *
 * Returns a pointer to the queue entry. or NULL if the queue is full.
 */
__host__ __device__
struct command* sq_enqueue(nvm_queue_t* sq);


/* Poll completion queue
 *
 * Check the head of a completion queue for a new entry. The caller must
 * call dequeue manually.
 *
 * Returns a pointer to an unprocessed completion entry, or NULL if the queue
 * is empty.
 */
__host__ __device__
struct completion* cq_poll(const nvm_queue_t* cq);


/* Dequeue completion queue entry
 *
 * Dequeue a completion entry from the completion queue. If there is no ready
 * completions, this function returns NULL.
 *
 * The caller must update the corresponding SQ manually.
 *
 * Returns a pointer to the completion entry, or NULL if the queue is empty.
 */
__host__ __device__
struct completion* cq_dequeue(nvm_queue_t* cq);


/* Dequeue completion queue entry
 *
 * Dequeue a completion entry from the completion queue. If none are ready
 * at the time, this function will block until a controller timeout interval
 * or a ready completion. 
 *
 * Returns a pointer to the completion entry, or NULL if the queue is empty.
 *
 * Note that this can only be called from the host, as it makes no sense to
 * block a CUDA kernel.
 */
__host__
struct completion* cq_dequeue_block(nvm_queue_t* cq, uint64_t timeout);


/* Update SQ tail pointer
 *
 * Submit all enqueued commands by ringing the doorbell.
 * The caller must make sure that all commands are prepared before calling
 * this.
 */
__host__ __device__
void sq_submit(const nvm_queue_t* sq);


/* Update SQ head pointer
 *
 * Update SQ head pointer according to the head pointer field in a completion.
 */
__host__ __device__
int sq_update(nvm_queue_t* sq, const struct completion* cpl);


/* Update controller's CQ head pointer
 *
 * Indicate that all completions are processed by ringing the doorbell.
 * All completion pointers acquired before this must be discarded after
 * calling this.
 */
__host__  __device__
void cq_update(const nvm_queue_t* cq); 


#ifdef __cplusplus
}
#endif
#endif