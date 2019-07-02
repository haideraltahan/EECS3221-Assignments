typedef struct queue_struct
{
  pthread_mutex_t qmutex;
  int n;
} queue_t;

extern queue_t *mk_queue();
extern void free_queue(queue_t *q);
extern void push_q(queue_t *q);
extern void pop_q(queue_t *q);
extern int safepop_q(queue_t *q);
extern int size_q(queue_t *q);
