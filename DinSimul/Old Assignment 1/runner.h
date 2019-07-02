
typedef struct thread_info thread_info;

struct thread_info
{
  int cnt;
  int who;
  thread_info *other;
};

extern void *runner(void *info);
