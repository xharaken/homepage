#ifndef _common_
#define _common_
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <netdb.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>
#include <malloc.h>
#include <regex.h>
#include <libgen.h>
#include <time.h>
#include <utime.h>
#include <setjmp.h>
#include <ucontext.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define FALSE 0
#define TRUE 1
#define UNDEF -1
#define SUCCESS 0
#define FAILURE 1

#define PIPE_OUT 0
#define PIPE_IN 1

#define PI 3.14159265358979323846264338327950288
#define E 2.71828182845904523536028747135266249
#define EPS6 1e-6
#define EPS12 1e-12
#define FNAME_SIZE 256
#define PAGESIZE 4096

#define SYS_MRAND_A 645
#define SYS_MRAND_B 1234567
#define SYS_MRAND_X0 137
#define SYS_MRAND_E32 (1.0 / (1 << 16) / (1 << 16))
#define SYS_MRAND_E64 (1.0 / (1 << 16) / (1 << 16) / (1 << 16) / (1 << 16))
#define SYS_LAP_TIME_MAX 128
#define SYS_MRAND_BUF_SIZE (sizeof(uint64_t) * 521 + sizeof(int8_t) + sizeof(int64_t) * 2)
#define SYS_TIME_BUF_SIZE (SYS_LAP_TIME_MAX * sizeof(double))

#define SET_TCP_NODELAY 1

#define IP_SIZE 128
#define SELF_IP "127.0.0.1"

#define CONTAINER_DEFAULT -1
#define VECTOR_INIT_CAPACITY 256
#define VECTOR_EXPAND_RATE 2.0
#define WRITEBUF_INIT_CAPACITY 1024
#define WRITEBUF_EXPAND_RATE 2.0
#define IDPOOL_INIT_CAPACITY 256
#define LEASE_INIT_CAPACITY CONTAINER_DEFAULT
#define LEASE_EXPAND_RATE CONTAINER_DEFAULT
#define DEQUE_INIT_CAPACITY 256
#define DEQUE_EXPAND_RATE 2.0

#define error() print_error(__FILE__, __LINE__)
#define throw(ret) if((ret) == FALSE) { printf("backtrace: %d @ %s\n", __LINE__, __FILE__); return (ret); }
#define catch(ret) if((ret) == FALSE) error()

typedef struct host_t
{
  char ip[IP_SIZE];
  uint16_t port;
}host_t;

typedef struct vector_t
{
  int32_t init_capacity;
  int32_t capacity;
  int32_t size;
  float expand_rate;
  void *undef_elem;
  void **buf;
}vector_t;

typedef struct xvector_t
{
  pthread_mutex_t mutex;
  vector_t *vector;
}xvector_t;

typedef struct readbuf_t
{
  int8_t *buf;
  int64_t offset;
  int64_t size;
}readbuf_t;

typedef struct xreadbuf_t
{
  pthread_mutex_t mutex;
  readbuf_t *readbuf;
}xreadbuf_t;

typedef struct writebuf_t
{
  int8_t *buf;
  int64_t size;
  int64_t capacity;
  float expand_rate;
}writebuf_t;

typedef struct xwritebuf_t
{
  pthread_mutex_t mutex;
  writebuf_t *writebuf;
}xwritebuf_t;

typedef struct idpool_t
{
  int32_t init_capacity;
  int32_t capacity;
  int32_t sp;
  int32_t size;
  int32_t *stack;
  int8_t *used_flag;
}idpool_t;

typedef struct xidpool_t
{
  pthread_mutex_t mutex;
  idpool_t *idpool;
}xidpool_t;

typedef struct lease_t
{
  int32_t init_capacity;
  float expand_rate;
  idpool_t *idpool;
  vector_t *vector;
}lease_t;

typedef struct xlease_t
{
  pthread_mutex_t mutex;
  lease_t *lease;
}xlease_t;

typedef struct deque_t
{
  int32_t capacity;
  int32_t init_capacity;
  int32_t head;
  int32_t tail;
  float expand_rate;
  void **buf;
}deque_t;

typedef struct xdeque_t
{
  pthread_mutex_t mutex;
  deque_t *deque;
}xdeque_t;

typedef struct taskque_t
{
  int8_t notify_flag;
  deque_t *deque;
  void *notify_elem;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
}taskque_t;

typedef struct cell_t
{
  void *elem;
  struct cell_t *prev;
  struct cell_t *next;
}cell_t;

typedef struct list_t
{
  int32_t size;
  int32_t pos;
  cell_t *head;
  cell_t *iter;
}list_t;

typedef struct xlist_t
{
  pthread_mutex_t mutex;
  list_t *list;
}xlist_t;

extern __thread double _tls_lap_times[SYS_LAP_TIME_MAX];
extern __thread uint64_t _tls_mrand_x[521];
extern __thread int8_t _tls_mrand_init_flag;
extern __thread uint64_t _tls_mrand_cur;
extern __thread uint64_t _tls_mrand_cur2;

extern void* __attribute__((weak)) my_malloc_hook(int64_t size);
extern void* __attribute__((weak)) my_realloc_hook(void *old_p, int64_t size);
extern void __attribute__((weak)) my_free_hook(void *p);

/* basic */

void print_error(const char *file, int line);
void btrace(void);
void out(const char *format, ...);
void outn(const char *format, ...);
void err(const char *format, ...);
void errn(const char *format, ...);
void* my_malloc(int64_t size);
void* my_calloc(int64_t size);
void* my_realloc(void *old_p, int64_t size);
void my_free(void *p);
double get_time(void);
void time_lap(int i);
double time_ref(int i);
double time_diff(int i);
void time_snapshot(int8_t *buf);
void time_resume(int8_t *buf);
void mrand_init(int64_t x0);
double mrand_01(void);
void mrand_snapshot(int8_t *buf);
void mrand_resume(int8_t *buf);
int64_t mrand_int(int64_t inf, int64_t sup);
void halt(double time);
int32_t my_write(int fd, void *p, int32_t size);
int32_t my_read(int fd, void *p, int32_t size);

/* socket */

int32_t sock_listen(uint16_t port, int32_t backlog);
int32_t sock_accept(int listen_sock);
void sock_close(int sock);
int32_t sock_connect(char *ip, uint16_t port);
int32_t sock_send(int sock, void *p, int32_t size);
int32_t sock_recv(int sock, void *p, int32_t size);
int32_t sock_create_udp(void);
int32_t sock_bind_udp(uint16_t port);
void sock_addr_udp(char *ip, uint16_t port, struct sockaddr_in *addr);
int32_t sock_send_udp(int sock, void *p, int32_t size, struct sockaddr_in *addr);
int32_t sock_recv_udp(int sock, void *p, int32_t size, struct sockaddr_in *addr);
void sock_get_myhost(int sock, host_t *host);
void sock_get_yourhost(int sock, host_t *host);
void sock_print_myhost(int sock);
void sock_print_yourhost(int sock);
void fqdn_to_ip(char *fqdn, char *ip, int32_t size);
int sock_create_epoll(int32_t size);
void sock_add_epoll(int sock, int epoll);
void sock_del_epoll(int sock, int epoll);
uint64_t htonll(uint64_t ull);
uint64_t ntohll(uint64_t n_ull);

/* vector */

vector_t* vector_alloc(int32_t init_capacity, float expand_rate, void *undef_elem);
void vector_free(vector_t *vector);
void vector_clear(vector_t *vector);
int32_t vector_isempty(vector_t *vector);
int32_t vector_size(vector_t *vector);
void vector_push(vector_t *vector, void *elem);
void* vector_pop(vector_t *vector);
void* vector_front(vector_t *vector);
void* vector_back(vector_t *vector);
void* vector_at(vector_t *vector, int32_t pos);
void vector_assign(vector_t *vector, int32_t pos, void *elem);
xvector_t* xvector_alloc(int32_t init_capacity, float expand_rate, void *undef_elem);
void xvector_free(xvector_t *xvector);
void xvector_clear(xvector_t *xvector);
int32_t xvector_isempty(xvector_t *xvector);
int32_t xvector_size(xvector_t *xvector);
void xvector_push(xvector_t *xvector, void *elem);
void* xvector_pop(xvector_t *xvector);
void* xvector_front(xvector_t *xvector);
void* xvector_back(xvector_t *xvector);
void* xvector_at(xvector_t *xvector, int32_t pos);
void xvector_assign(xvector_t *xvector, int32_t pos, void *elem);

/* readbuf */

readbuf_t* readbuf_alloc(void *buf, int64_t size);
void readbuf_free(readbuf_t *readbuf);
void* readbuf_buf(readbuf_t *readbuf);
int64_t readbuf_offset(readbuf_t *readbuf);
void readbuf_copy(readbuf_t *readbuf, void *elem, int64_t elem_size);
xreadbuf_t* xreadbuf_alloc(void *buf, int64_t size);
void xreadbuf_free(xreadbuf_t *xreadbuf);
void* xreadbuf_buf(xreadbuf_t *xreadbuf);
int64_t xreadbuf_offset(xreadbuf_t *xreadbuf);
void xreadbuf_copy(xreadbuf_t *xreadbuf, void *elem, int64_t elem_size);

/* writebuf */

writebuf_t* writebuf_alloc(int64_t init_capacity, float expand_rate);
void writebuf_free(writebuf_t *writebuf);
int64_t writebuf_size(writebuf_t *writebuf);
void* writebuf_buf(writebuf_t *writebuf);
void writebuf_copy(writebuf_t *writebuf, void *elem, int64_t elem_size);
int64_t writebuf_skip(writebuf_t *writebuf, int64_t size);
void writebuf_seekcopy(writebuf_t *writebuf, int64_t offset, void *elem, int64_t elem_size);
xwritebuf_t* xwritebuf_alloc(int64_t init_capacity, float expand_rate);
void xwritebuf_free(xwritebuf_t *xwritebuf);
int64_t xwritebuf_size(xwritebuf_t *xwritebuf);
void* xwritebuf_buf(xwritebuf_t *xwritebuf);
void xwritebuf_copy(xwritebuf_t *xwritebuf, void *elem, int64_t elem_size);
int64_t xwritebuf_skip(xwritebuf_t *xwritebuf, int64_t size);
void xwritebuf_seekcopy(xwritebuf_t *xwritebuf, int64_t offset, void *elem, int64_t elem_size);

/* idpool */

idpool_t* idpool_alloc(int32_t init_capacity);
void idpool_free(idpool_t *idpool);
int32_t idpool_isempty(idpool_t *idpool);
int32_t idpool_size(idpool_t *idpool);
int32_t idpool_get(idpool_t *idpool);
void idpool_put(idpool_t *idpool, int32_t id);
xidpool_t* xidpool_alloc(int32_t init_capacity);
void xidpool_free(xidpool_t *xidpool);
int32_t xidpool_isempty(xidpool_t *xidpool);
int32_t xidpool_size(xidpool_t *xidpool);
int32_t xidpool_get(xidpool_t *xidpool);
void xidpool_put(xidpool_t *xidpool, int32_t id);

/* lease */

lease_t* lease_alloc(int32_t init_capacity, float expand_rate, void *undef_elem);
void lease_free(lease_t *lease);
int lease_isempty(lease_t *lease);
int32_t lease_size(lease_t *lease);
int32_t lease_putin(lease_t *lease, void *elem);
void* lease_pickup(lease_t *lease, int32_t id);
void* lease_at(lease_t *lease, int32_t pos);
xlease_t* xlease_alloc(int32_t init_capacity, float expand_rate, void *undef_elem);
void xlease_free(xlease_t *xlease);
int xlease_isempty(xlease_t *xlease);
int32_t xlease_size(xlease_t *xlease);
int32_t xlease_putin(xlease_t *xlease, void *elem);
void* xlease_pickup(xlease_t *xlease, int32_t id);
void* xlease_at(xlease_t *xlease, int32_t pos);

/* deque */

deque_t* deque_alloc(int32_t init_capacity, float expand_rate);
void deque_free(deque_t *deque);
void deque_clear(deque_t *deque);
int32_t deque_isempty(deque_t *deque);
int32_t deque_size(deque_t *deque);
void* deque_front(deque_t *deque);
void* deque_back(deque_t *deque);
void deque_push(deque_t *deque, void *elem);
void* deque_pop(deque_t *deque);
void* deque_shift(deque_t *deque);
void deque_unshift(deque_t *deque, void *elem);
void* deque_at(deque_t *deque, int32_t pos);
void deque_assign(deque_t *deque, int32_t pos, void *elem);
xdeque_t* xdeque_alloc(int32_t init_capacity, float expand_rate);
void xdeque_free(xdeque_t *xdeque);
void xdeque_clear(xdeque_t *xdeque);
int32_t xdeque_isempty(xdeque_t *xdeque);
int32_t xdeque_size(xdeque_t *xdeque);
void* xdeque_front(xdeque_t *xdeque);
void* xdeque_back(xdeque_t *xdeque);
void xdeque_push(xdeque_t *xdeque, void *elem);
void* xdeque_pop(xdeque_t *xdeque);
void* xdeque_shift(xdeque_t *xdeque);
void xdeque_unshift(xdeque_t *xdeque, void *elem);
void* xdeque_at(xdeque_t *xdeque, int32_t pos);
void xdeque_assign(xdeque_t *xdeque, int32_t pos, void *elem);

/* taskque */

taskque_t* taskque_alloc(int32_t init_capacity, float expand_rate, void *notify_elem);
void taskque_free(taskque_t *taskque);
int32_t taskque_isempty(taskque_t *taskque);
int32_t taskque_size(taskque_t *taskque);
void taskque_unshift(taskque_t *taskque, void *elem);
void* taskque_pop(taskque_t *taskque);
void* taskque_at(taskque_t *taskque, int32_t pos);
void taskque_notify(taskque_t *taskque);

/* list */

cell_t* cell_alloc(void *elem);
void cell_free(cell_t *cell);
list_t* list_alloc(void);
void list_free(list_t *list);
void list_clear(list_t *list);
void list_head(list_t *list);
void list_tail(list_t *list);
int32_t list_isempty(list_t *list);
int32_t list_size(list_t *list);
void* list_curr(list_t *list);
void* list_next(list_t *list);
void* list_prev(list_t *list);
int32_t list_hascurr(list_t *list);
int32_t list_hasnext(list_t *list);
int32_t list_hasprev(list_t *list);
void list_push(list_t *list, void *elem);
void* list_pop(list_t *list);
void* list_shift(list_t *list);
void list_unshift(list_t *list, void *elem);
void list_insert(list_t *list, void *elem);
void* list_remove(list_t *list);
void list_move(list_t *list, int32_t pos);
void* list_at(list_t *list, int32_t pos);
void list_assign(list_t *list, int32_t pos, void *elem);
xlist_t* xlist_alloc(void);
void xlist_free(xlist_t *xlist);
void xlist_clear(xlist_t *xlist);
void xlist_head(xlist_t *xlist);
void xlist_tail(xlist_t *xlist);
int32_t xlist_isempty(xlist_t *xlist);
int32_t xlist_size(xlist_t *xlist);
void* xlist_curr(xlist_t *xlist);
void* xlist_next(xlist_t *xlist);
void* xlist_prev(xlist_t *xlist);
int32_t xlist_hascurr(xlist_t *xlist);
int32_t xlist_hasnext(xlist_t *xlist);
int32_t xlist_hasprev(xlist_t *xlist);
void xlist_push(xlist_t *xlist, void *elem);
void* xlist_pop(xlist_t *xlist);
void* xlist_shift(xlist_t *xlist);
void xlist_unshift(xlist_t *xlist, void *elem);
void xlist_insert(xlist_t *xlist, void *elem);
void* xlist_remove(xlist_t *xlist);
void xlist_move(xlist_t *xlist, int32_t pos);
void* xlist_at(xlist_t *xlist, int32_t pos);
void xlist_assign(xlist_t *xlist, int32_t pos, void *elem);


#endif
