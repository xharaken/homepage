#include "common.h"

__thread double _tls_time_laps[SYS_LAP_TIME_MAX];
__thread uint64_t _tls_mrand_x[521];
__thread int8_t _tls_mrand_init_flag = FALSE;
__thread uint64_t _tls_mrand_cur;
__thread uint64_t _tls_mrand_cur2;

/* basic */

void print_error(const char *file, int line)
{
  /*
  int32_t ret;
  */
  
  fprintf(stderr, "[error] %s @ %s : %d\n", strerror(errno), file, line);
  fflush(stdout);
  /*
  ret = kill(getpid(), SIGQUIT);
  if(ret < 0) exit(1);
  */
  exit(1);
  return;
}

void btrace(void)
{
  int n;
  void *buf[4096];
  
  n = backtrace(buf, 4096);
  backtrace_symbols_fd(buf, n, 1);
  return;
}

void out(const char *format, ...)
{
  va_list va_arg;
  
  va_start(va_arg, format);
  vprintf(format, va_arg);
  fflush(stdout);
  va_end(va_arg);
  return;
}

void outn(const char *format, ...)
{
  va_list va_arg;
  
  va_start(va_arg, format);
  vprintf(format, va_arg);
  printf("\n");
  fflush(stdout);
  va_end(va_arg);
  return;
}

void err(const char *format, ...)
{
  va_list va_arg;
  
  va_start(va_arg, format);
  vfprintf(stderr, format, va_arg);
  fflush(stderr);
  va_end(va_arg);
  return;
}

void errn(const char *format, ...)
{
  va_list va_arg;
  
  va_start(va_arg, format);
  vfprintf(stderr, format, va_arg);
  fprintf(stderr, "\n");
  fflush(stderr);
  va_end(va_arg);
  return;
}

void* my_malloc(int64_t size)
{
  void *p;
  
  if(my_malloc_hook)
    {
      p = my_malloc_hook(size);
    }
  else
    {
      p = malloc(size);
      /*p = calloc(size, 1);*/
    }
  if(p == NULL) error();
  return p;
}

void* my_calloc(int64_t size)
{
  void *p;
  
  p = calloc(size, 1);
  if(size != 0 && p == NULL) error();
  return p;
}

void* my_realloc(void *old_p, int64_t size)
{
  void *p;
  
  if(my_realloc_hook)
    {
      p = my_realloc_hook(old_p, size);
    }
  else
    {
      p = realloc(old_p, size);
    }
  if(size != 0 && p == NULL) error();
  return p;
}

void my_free(void *p)
{
  if(my_free_hook)
    {
      my_free_hook(p);
    }
  else
    {
      free(p);
    }
  return;
}

double get_time(void)
{
  struct timeval tv;
  
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

void time_lap(int i)
{
  struct timeval tv;
  
  if(!(0 <= i && i < SYS_LAP_TIME_MAX)) error();
  gettimeofday(&tv, NULL);
  _tls_time_laps[i] = tv.tv_sec + tv.tv_usec * 1e-6;
  return;
}

double time_ref(int i)
{
  if(!(0 <= i && i < SYS_LAP_TIME_MAX)) error();
  return _tls_time_laps[i];
}

double time_diff(int i)
{
  struct timeval tv;
  
  if(!(0 <= i && i < SYS_LAP_TIME_MAX)) error();
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6 - _tls_time_laps[i];
}

void time_snapshot(int8_t *buf)
{
  int32_t offset, size;
  
  offset = 0;
  size = SYS_LAP_TIME_MAX * sizeof(double);
  memcpy(buf + offset, _tls_time_laps, size);
  offset += size;
  return;
}

void time_resume(int8_t *buf)
{
  int32_t offset, size;
  
  offset = 0;
  size = SYS_LAP_TIME_MAX * sizeof(double);
  memcpy(_tls_time_laps, buf + offset, size);
  offset += size;
  return;
}

void mrand_init(int64_t x0)
{
  int64_t i;
  
  _tls_mrand_x[0] = SYS_MRAND_A * x0;
  for(i = 1; i < 521; i++)
    {
      _tls_mrand_x[i] = SYS_MRAND_A * _tls_mrand_x[i - 1] + SYS_MRAND_B;
    }
  _tls_mrand_init_flag = TRUE;
  _tls_mrand_cur = 0;
  _tls_mrand_cur2 = 521;
  return;
}

double mrand_01(void)
{
  int64_t i;
  double d;
  
  if(_tls_mrand_init_flag == FALSE)
    {
      _tls_mrand_x[0] = SYS_MRAND_A * SYS_MRAND_X0;
      for(i = 1; i < 521; i++)
        {
          _tls_mrand_x[i] = SYS_MRAND_A * _tls_mrand_x[i - 1];
        }
      _tls_mrand_init_flag = TRUE;
      _tls_mrand_cur = 0;
      _tls_mrand_cur2 = 521;
    }
  
  _tls_mrand_x[_tls_mrand_cur] = _tls_mrand_x[_tls_mrand_cur] ^ _tls_mrand_x[_tls_mrand_cur2 - 32];
  d = _tls_mrand_x[_tls_mrand_cur] * SYS_MRAND_E64;
  _tls_mrand_cur++;
  _tls_mrand_cur2++;
  if(_tls_mrand_cur == 521)
    {
      _tls_mrand_cur = 0;
    }
  if(_tls_mrand_cur2 == 553)
    {
      _tls_mrand_cur2 = 32;
    }
  return d;
}

int64_t mrand_int(int64_t inf, int64_t sup)
{
  return inf + (int64_t)(mrand_01() * (sup - inf + 1));
}

void mrand_snapshot(int8_t *buf)
{
  int64_t offset, size;
  
  offset = 0;
  size = sizeof(int8_t);
  memcpy(buf + offset, &_tls_mrand_init_flag, size);
  offset += size;
  size = sizeof(int64_t);
  memcpy(buf + offset, &_tls_mrand_cur, size);
  offset += size;
  size = sizeof(int64_t);
  memcpy(buf + offset, &_tls_mrand_cur2, size);
  offset += size;
  size = 521 * sizeof(uint64_t);
  memcpy(buf + offset, _tls_mrand_x, size);
  offset += size;
  return;
}

void mrand_resume(int8_t *buf)
{
  int64_t offset, size;
  
  offset = 0;
  size = sizeof(int8_t);
  memcpy(&_tls_mrand_init_flag, buf + offset, size);
  offset += size;
  size = sizeof(int64_t);
  memcpy(&_tls_mrand_cur, buf + offset, size);
  offset += size;
  size = sizeof(int64_t);
  memcpy(&_tls_mrand_cur2, buf + offset, size);
  offset += size;
  size = 521 * sizeof(uint64_t);
  memcpy(_tls_mrand_x, buf + offset, size);
  offset += size;
  return;
}

void halt(double time)
{
  struct timespec spec;
  
  spec.tv_sec = (int)time;
  spec.tv_nsec = (int)((time - (int)time) * 1e9);
  nanosleep(&spec, NULL);
  return;
}

int32_t my_write(int fd, void *p, int32_t size)
{
  int32_t ret;
  
  ret = write(fd, (int8_t*)p, size);
  if(ret != size) return -1;
  if(ret < 0) return ret;
  return ret;
}

int32_t my_read(int fd, void *p, int32_t size)
{
  int32_t sum, ret;
  int8_t *tmp_p;
  
  tmp_p = (int8_t*)p;
  ret = 0;
  for(sum = 0; sum < size; sum += ret)
    {
      ret = read(fd, tmp_p + sum, size - sum);
      if(ret == 0) return ret;
      if(ret < 0) return ret;
    }
  return sum;
}

/* socket */

int32_t sock_listen(uint16_t port, int32_t backlog)
{
  struct sockaddr_in addr;
  int listen_sock;
  int32_t ret, value;
  
  listen_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(listen_sock < 0) error();
  
  value = 1;
  if(setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int32_t)) < 0) error();
  
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  
  ret = bind(listen_sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
  if(ret < 0) return ret;
  
  ret = listen(listen_sock, backlog);
  if(ret < 0) return ret;
  
  return listen_sock;
}

int32_t sock_accept(int listen_sock)
{
  struct sockaddr_in addr;
  socklen_t addr_size;
  int sock;
  int32_t value;
  
  addr_size = sizeof(struct sockaddr);
  sock = accept(listen_sock, (struct sockaddr*)&addr, (socklen_t*)&addr_size);
  if(sock < 0) return sock;
  
#if SET_TCP_NODELAY
  value = 1;
  if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(int32_t)) < 0) error();
#endif
  
  return sock;
}

void sock_close(int sock)
{
  int32_t ret;
  
  ret = close(sock);
  if(ret < 0) error();
  return;
}

int32_t sock_connect(char *ip, uint16_t port)
{
  struct sockaddr_in addr;
  int sock;
  int32_t ret, value;
  
  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sock < 0) error();
  
#if SET_TCP_NODELAY
  value = 1;
  if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(int32_t)) < 0) error();
#endif
  
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);
  
  ret = connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr));
  if(ret < 0) return ret;
  
  return sock;
}

int32_t sock_send(int sock, void *p, int32_t size)
{
  int32_t ret;
  
  ret = send(sock, (int8_t*)p, size, 0);
  if(ret != size) return -1;
  if(ret < 0) return ret;
  return ret;
}

int32_t sock_recv(int sock, void *p, int32_t size)
{
  int32_t sum, ret;
  int8_t *tmp_p;
  
  tmp_p = (int8_t*)p;
  ret = 0;
  for(sum = 0; sum < size; sum += ret)
    {
      ret = recv(sock, tmp_p + sum, size - sum, 0);
      if(ret == 0) return ret;
      if(ret < 0) return ret;
    }
  return sum;
}

int32_t sock_create_udp(void)
{
  int sock;
  
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sock < 0) error();
  return sock;
}

int32_t sock_bind_udp(uint16_t port)
{
  struct sockaddr_in addr;
  int sock;
  int32_t ret;
  
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sock < 0) error();
  
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  
  ret = bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
  if(ret < 0) return ret;
  return sock;
}

void sock_addr_udp(char *ip, uint16_t port, struct sockaddr_in *addr)
{
  socklen_t addr_size;
  
  addr_size = sizeof(struct sockaddr_in);
  memset(addr, 0, addr_size);
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr(ip);
  addr->sin_port = htons(port);
  return;
}

int32_t sock_send_udp(int sock, void *p, int32_t size, struct sockaddr_in *addr)
{
  socklen_t addr_size;
  int32_t ret;
  
  addr_size = sizeof(struct sockaddr_in);
  ret = sendto(sock, p, size, 0, (struct sockaddr*)addr, addr_size);
  if(ret < 0) return ret;
  return ret;
}

int32_t sock_recv_udp(int sock, void *p, int32_t size, struct sockaddr_in *addr)
{
  socklen_t addr_size;
  int32_t ret;
  
  addr_size = sizeof(struct sockaddr_in);
  ret = recvfrom(sock, p, size, 0, (struct sockaddr*)addr, &addr_size);
  if(ret < 0) return ret;
  return ret;
}

void sock_get_myhost(int sock, host_t *host)
{
  struct sockaddr_in addr;
  socklen_t addr_size;
  int32_t ret;
  
  addr_size = sizeof(struct sockaddr_in);
  ret = getsockname(sock, (struct sockaddr*)&addr, (socklen_t*)&addr_size);
  if(ret < 0) error();
  strncpy(host->ip, inet_ntoa(addr.sin_addr), IP_SIZE);
  host->ip[IP_SIZE - 1] = 0;
  host->port = ntohs(addr.sin_port);
  return;
}

void sock_get_yourhost(int sock, host_t *host)
{
  struct sockaddr_in addr;
  socklen_t addr_size;
  int32_t ret;
  
  addr_size = sizeof(struct sockaddr_in);
  ret = getpeername(sock, (struct sockaddr*)&addr, (socklen_t*)&addr_size);
  if(ret < 0) error();
  strncpy(host->ip, inet_ntoa(addr.sin_addr), IP_SIZE);
  host->ip[IP_SIZE - 1] = 0;
  host->port = ntohs(addr.sin_port);
  return;
}

void sock_print_myhost(int sock)
{
  host_t host;
  
  sock_get_myhost(sock, &host);
  printf("local : %s @ %d\n", host.ip, (int32_t)host.port);
  return;
}

void sock_print_yourhost(int sock)
{
  host_t host;
  
  sock_get_yourhost(sock, &host);
  printf("remote : %s @ %d\n", host.ip, (int32_t)host.port);
  return;
}

void fqdn_to_ip(char *fqdn, char *ip, int32_t size)
{
  struct addrinfo hints;
  struct addrinfo *res;
  struct in_addr in;
  int32_t ret;
  
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  
  ret = getaddrinfo(fqdn, NULL, NULL, &res);
  if(ret < 0) error();
  in = ((struct sockaddr_in*)res->ai_addr)->sin_addr;
  strncpy(ip, inet_ntoa(in), size);
  return;
}

int sock_create_epoll(int32_t size)
{
  int epoll;
  
  epoll = epoll_create(size);
  if(epoll < 0) error();
  return epoll;
}

void sock_add_epoll(int epoll, int sock)
{
  struct epoll_event new_event;
  int32_t ret;
  
  new_event.events = EPOLLIN;
  new_event.data.fd = sock;
  ret = epoll_ctl(epoll, EPOLL_CTL_ADD, sock, &new_event);
  if(ret < 0) error();
  return;
}

void sock_del_epoll(int epoll, int sock)
{
  int32_t ret;
  
  ret = epoll_ctl(epoll, EPOLL_CTL_DEL, sock, NULL);
  if(ret < 0) error();
  return;
}

/* this is wrong */
uint64_t htonll(uint64_t ull)
{
  uint64_t upper, lower, n_upper, n_lower, n_ull;
  
  upper = ull >> 32;
  lower = ull & 0xffffffffUL;
  n_upper = htonl(upper);
  n_lower = htonl(lower);
  n_ull = (n_upper << 32UL) | n_lower;
  return n_ull;
}

/* this is wrong */
uint64_t ntohll(uint64_t n_ull)
{
  uint64_t upper, lower, n_upper, n_lower, ull;
  
  n_upper = n_ull >> 32;
  n_lower = n_ull & 0xffffffffUL;
  upper = ntohl(n_upper);
  lower = ntohl(n_lower);
  ull = (upper << 32UL) | lower;
  return ull;
}

/* vector */

vector_t* vector_alloc(int32_t init_capacity, float expand_rate, void *undef_elem)
{
  vector_t *vector;
  
  if(init_capacity == CONTAINER_DEFAULT) init_capacity = VECTOR_INIT_CAPACITY;
  if(expand_rate == CONTAINER_DEFAULT) expand_rate = VECTOR_EXPAND_RATE;
  
  vector = (vector_t*)my_malloc(sizeof(vector_t));
  vector->init_capacity = init_capacity;
  vector->capacity = vector->init_capacity;
  vector->size = 0;
  vector->expand_rate = expand_rate;
  vector->undef_elem = undef_elem;
  vector->buf = (void**)my_malloc(vector->capacity * sizeof(void*));
  return vector;
}

void vector_free(vector_t *vector)
{
  my_free(vector->buf);
  my_free(vector);
  return;
}

void vector_clear(vector_t *vector)
{
  my_free(vector->buf);
  vector->capacity = vector->init_capacity;
  vector->buf = (void**)my_malloc(vector->capacity * sizeof(void*));
  vector->size = 0;
  return;
}

int32_t vector_isempty(vector_t *vector)
{
  int32_t ret;
  
  ret = vector->size == 0 ? TRUE : FALSE;
  return ret;
}

int32_t vector_size(vector_t *vector)
{
  int32_t size;
  
  size = vector->size;
  return size;
}

void vector_push(vector_t *vector, void *elem)
{
  int32_t pos;
  
  pos = vector->size + 1;
  if(pos >= vector->capacity)
    {
      vector->capacity = pos;
      vector->capacity = vector->capacity * vector->expand_rate;
      vector->buf = (void**)my_realloc(vector->buf, vector->capacity * sizeof(void*));
    }
  vector->buf[vector->size++] = elem;
  return;
}

void* vector_pop(vector_t *vector)
{
  void *elem;
  
  if(vector->size <= 0) error();
  
  elem = vector->buf[--vector->size];
  return elem;
}

void* vector_front(vector_t *vector)
{
  void *elem;
  
  if(vector->size <= 0) error();
  
  elem = vector->buf[0];
  return elem;
}

void* vector_back(vector_t *vector)
{
  void *elem;
  
  if(vector->size <= 0) error();
  
  elem = vector->buf[vector->size - 1];
  return elem;
}

void* vector_at(vector_t *vector, int32_t pos)
{
  void *elem;
  
  if(0 <= pos && pos < vector->size)
    {
      elem = vector->buf[pos];
    }
  else
    {
      elem = vector->undef_elem;
    }
  return elem;
}

void vector_assign(vector_t *vector, int32_t pos, void *elem)
{
  int32_t i;
  
  if(pos < 0) error();
  
  if(pos < vector->size)
    {
      vector->buf[pos] = elem;
    }
  else
    {
      vector->capacity = pos + 1;
      vector->capacity = vector->capacity * vector->expand_rate;
      vector->buf = (void**)my_realloc(vector->buf, sizeof(void*) * vector->capacity);
      for(i = vector->size; i < pos; i++)
        {
          vector->buf[i] = vector->undef_elem;
        }
      vector->buf[pos] = elem;
      vector->size = pos + 1;
    }
  return;
}

xvector_t* xvector_alloc(int32_t init_capacity, float expand_rate, void *undef_elem)
{
  xvector_t *xvector;
  
  xvector = (xvector_t*)my_malloc(sizeof(xvector_t));
  xvector->vector = vector_alloc(init_capacity, expand_rate, undef_elem);
  pthread_mutex_init(&xvector->mutex, NULL);
  return xvector;
}

void xvector_free(xvector_t *xvector)
{
  pthread_mutex_destroy(&xvector->mutex);
  vector_free(xvector->vector);
  my_free(xvector);
  return;
}

void xvector_clear(xvector_t *xvector)
{
  pthread_mutex_lock(&xvector->mutex);
  vector_clear(xvector->vector);
  pthread_mutex_unlock(&xvector->mutex);
  return;
}

int32_t xvector_isempty(xvector_t *xvector)
{
  int32_t ret;
  
  pthread_mutex_lock(&xvector->mutex);
  ret = vector_isempty(xvector->vector);
  pthread_mutex_unlock(&xvector->mutex);
  return ret;
}

int32_t xvector_size(xvector_t *xvector)
{
  int32_t size;
  
  pthread_mutex_lock(&xvector->mutex);
  size = vector_size(xvector->vector);
  pthread_mutex_unlock(&xvector->mutex);
  return size;
}

void xvector_push(xvector_t *xvector, void *elem)
{
  pthread_mutex_lock(&xvector->mutex);
  vector_push(xvector->vector, elem);
  pthread_mutex_unlock(&xvector->mutex);
  return;
}

void* xvector_pop(xvector_t *xvector)
{
  void *elem;
  
  pthread_mutex_lock(&xvector->mutex);
  elem = vector_pop(xvector->vector);
  pthread_mutex_unlock(&xvector->mutex);
  return elem;
}

void* xvector_front(xvector_t *xvector)
{
  void *elem;
  
  pthread_mutex_lock(&xvector->mutex);
  elem = vector_front(xvector->vector);
  pthread_mutex_unlock(&xvector->mutex);
  return elem;
}

void* xvector_back(xvector_t *xvector)
{
  void *elem;
  
  pthread_mutex_lock(&xvector->mutex);
  elem = vector_back(xvector->vector);
  pthread_mutex_unlock(&xvector->mutex);
  return elem;
}

void* xvector_at(xvector_t *xvector, int32_t pos)
{
  void *elem;
  
  pthread_mutex_lock(&xvector->mutex);
  elem = vector_at(xvector->vector, pos);
  pthread_mutex_unlock(&xvector->mutex);
  return elem;
}

void xvector_assign(xvector_t *xvector, int32_t pos, void *elem)
{
  pthread_mutex_lock(&xvector->mutex);
  vector_assign(xvector->vector, pos, elem);
  pthread_mutex_unlock(&xvector->mutex);
  return;
}

/* readbuf */

readbuf_t* readbuf_alloc(void *buf, int64_t size)
{
  readbuf_t *readbuf;
  
  readbuf = (readbuf_t*)my_malloc(sizeof(readbuf_t));
  readbuf->offset = 0;
  readbuf->buf = buf;
  readbuf->size = size;
  return readbuf;
}

void readbuf_free(readbuf_t *readbuf)
{
  my_free(readbuf);
  return;
}

void* readbuf_buf(readbuf_t *readbuf)
{
  void *buf;
  
  buf = readbuf->buf + readbuf->offset;
  return buf;
}

int64_t readbuf_offset(readbuf_t *readbuf)
{
  int64_t offset;
  
  offset = readbuf->offset;
  return offset;
}

void readbuf_copy(readbuf_t *readbuf, void *elem, int64_t elem_size)
{
  if(readbuf->offset + elem_size > readbuf->size)
    {
      error();
    }
  
  memcpy(elem, readbuf->buf + readbuf->offset, elem_size);
  readbuf->offset += elem_size;
  return;
}

xreadbuf_t* xreadbuf_alloc(void *buf, int64_t size)
{
  xreadbuf_t *xreadbuf;
  
  xreadbuf = (xreadbuf_t*)my_malloc(sizeof(xreadbuf_t));
  xreadbuf->readbuf = readbuf_alloc(buf, size);
  pthread_mutex_init(&xreadbuf->mutex, NULL);
  return xreadbuf;
}

void xreadbuf_free(xreadbuf_t *xreadbuf)
{
  pthread_mutex_destroy(&xreadbuf->mutex);
  readbuf_free(xreadbuf->readbuf);
  my_free(xreadbuf);
  return;
}

void* xreadbuf_buf(xreadbuf_t *xreadbuf)
{
  void *buf;
  
  pthread_mutex_lock(&xreadbuf->mutex);
  buf = readbuf_buf(xreadbuf->readbuf);
  pthread_mutex_unlock(&xreadbuf->mutex);
  return buf;
}

int64_t xreadbuf_offset(xreadbuf_t *xreadbuf)
{
  int64_t offset;
  
  pthread_mutex_lock(&xreadbuf->mutex);
  offset = readbuf_offset(xreadbuf->readbuf);
  pthread_mutex_unlock(&xreadbuf->mutex);
  return offset;
}

void xreadbuf_copy(xreadbuf_t *xreadbuf, void *elem, int64_t elem_size)
{
  pthread_mutex_lock(&xreadbuf->mutex);
  readbuf_copy(xreadbuf->readbuf, elem, elem_size);
  pthread_mutex_unlock(&xreadbuf->mutex);
  return;
}

/* writebuf */

writebuf_t* writebuf_alloc(int64_t init_capacity, float expand_rate)
{
  writebuf_t *writebuf;
  
  if(init_capacity == CONTAINER_DEFAULT) init_capacity = WRITEBUF_INIT_CAPACITY;
  if(expand_rate == CONTAINER_DEFAULT) expand_rate = WRITEBUF_EXPAND_RATE;
  
  writebuf = (writebuf_t*)my_malloc(sizeof(writebuf_t));
  writebuf->size = 0;
  writebuf->capacity = init_capacity;
  writebuf->buf = (int8_t*)my_malloc(writebuf->capacity);
  writebuf->expand_rate = expand_rate;
  return writebuf;
}

void writebuf_free(writebuf_t *writebuf)
{
  my_free(writebuf->buf);
  my_free(writebuf);
  return;
}

int64_t writebuf_size(writebuf_t *writebuf)
{
  int64_t size;
  
  size = writebuf->size;
  return size;
}

void* writebuf_buf(writebuf_t *writebuf)
{
  void *buf;
  
  buf = writebuf->buf;
  return buf;
}

void writebuf_copy(writebuf_t *writebuf, void *elem, int64_t elem_size)
{
  int64_t pos;
  
  pos = writebuf->size + elem_size;
  if(pos >= writebuf->capacity)
    {
      writebuf->capacity = pos;
      writebuf->capacity = writebuf->capacity * writebuf->expand_rate;
      writebuf->buf = (int8_t*)my_realloc(writebuf->buf, writebuf->capacity);
    }
  
  memcpy(writebuf->buf + writebuf->size, elem, elem_size);
  writebuf->size += elem_size;
  return;
}

int64_t writebuf_skip(writebuf_t *writebuf, int64_t size)
{
  int64_t pos, offset;
  
  pos = writebuf->size + size;
  if(pos >= writebuf->capacity)
    {
      writebuf->capacity = pos;
      writebuf->capacity = writebuf->capacity * writebuf->expand_rate;
      writebuf->buf = (int8_t*)my_realloc(writebuf->buf, writebuf->capacity);
    }
  
  offset = writebuf->size;
  writebuf->size += size;
  return offset;
}

void writebuf_seekcopy(writebuf_t *writebuf, int64_t offset, void *elem, int64_t elem_size)
{
  int64_t pos;
  
  pos = offset + elem_size;
  if(pos >= writebuf->capacity)
    {
      error();
    }
  
  memcpy(writebuf->buf + offset, elem, elem_size);
  return;
}

xwritebuf_t* xwritebuf_alloc(int64_t init_capacity, float expand_rate)
{
  xwritebuf_t *xwritebuf;
  
  xwritebuf = (xwritebuf_t*)my_malloc(sizeof(xwritebuf_t));
  xwritebuf->writebuf = writebuf_alloc(init_capacity, expand_rate);
  pthread_mutex_init(&xwritebuf->mutex, NULL);
  return xwritebuf;
}

void xwritebuf_free(xwritebuf_t *xwritebuf)
{
  pthread_mutex_destroy(&xwritebuf->mutex);
  writebuf_free(xwritebuf->writebuf);
  my_free(xwritebuf);
  return;
}

int64_t xwritebuf_size(xwritebuf_t *xwritebuf)
{
  int64_t size;
  
  pthread_mutex_lock(&xwritebuf->mutex);
  size = writebuf_size(xwritebuf->writebuf);
  pthread_mutex_unlock(&xwritebuf->mutex);
  return size;
}

void* xwritebuf_buf(xwritebuf_t *xwritebuf)
{
  void *buf;
  
  pthread_mutex_lock(&xwritebuf->mutex);
  buf = writebuf_buf(xwritebuf->writebuf);
  pthread_mutex_unlock(&xwritebuf->mutex);
  return buf;
}

void xwritebuf_copy(xwritebuf_t *xwritebuf, void *elem, int64_t elem_size)
{
  pthread_mutex_lock(&xwritebuf->mutex);
  writebuf_copy(xwritebuf->writebuf, elem, elem_size);
  pthread_mutex_unlock(&xwritebuf->mutex);
  return;
}

int64_t xwritebuf_skip(xwritebuf_t *xwritebuf, int64_t size)
{
  int64_t offset;
  
  pthread_mutex_lock(&xwritebuf->mutex);
  offset = writebuf_skip(xwritebuf->writebuf, size);
  pthread_mutex_unlock(&xwritebuf->mutex);
  return offset;
}

void xwritebuf_seekcopy(xwritebuf_t *xwritebuf, int64_t offset, void *elem, int64_t elem_size)
{
  pthread_mutex_lock(&xwritebuf->mutex);
  writebuf_seekcopy(xwritebuf->writebuf, offset, elem, elem_size);
  pthread_mutex_unlock(&xwritebuf->mutex);
  return;
}

/* idpool */

idpool_t* idpool_alloc(int32_t init_capacity)
{
  idpool_t *idpool;
  int32_t i;
  
  if(init_capacity == CONTAINER_DEFAULT) init_capacity = IDPOOL_INIT_CAPACITY;
  
  idpool = (idpool_t*)my_malloc(sizeof(idpool_t));
  idpool->size = 0;
  idpool->init_capacity = init_capacity;
  idpool->capacity = idpool->init_capacity;
  idpool->stack = (int32_t*)my_malloc(idpool->capacity * sizeof(int32_t));
  idpool->used_flag = (int8_t*)my_malloc(idpool->capacity * sizeof(int8_t));
  idpool->sp = 0;
  for(i = 0; i < idpool->init_capacity; i++)
    {
      idpool->stack[idpool->sp++] = idpool->capacity - i - 1;
      idpool->used_flag[idpool->capacity - i - 1] = FALSE;
    }
  return idpool;
}

void idpool_free(idpool_t *idpool)
{
  my_free(idpool->stack);
  my_free(idpool->used_flag);
  my_free(idpool);
  return;
}

int32_t idpool_isempty(idpool_t *idpool)
{
  int32_t ret;
  
  ret = idpool->size == 0 ? TRUE : FALSE;
  return ret;
}

int32_t idpool_size(idpool_t *idpool)
{
  int32_t size;
  
  size = idpool->size;
  return size;
}

int32_t idpool_get(idpool_t *idpool)
{
  int32_t i, id;
  
  if(idpool->sp == 0)
    {
      idpool->capacity += idpool->init_capacity;
      idpool->stack = (int32_t*)my_realloc(idpool->stack, idpool->capacity * sizeof(int32_t));
      idpool->used_flag = (int8_t*)my_realloc(idpool->used_flag, idpool->capacity * sizeof(int8_t));
      for(i = 0; i < idpool->init_capacity; i++)
        {
          idpool->stack[idpool->sp++] = idpool->capacity - i - 1;
          idpool->used_flag[idpool->capacity - i - 1] = FALSE;
        }
    }
  id = idpool->stack[--idpool->sp];
  idpool->used_flag[id] = TRUE;
  idpool->size++;
  return id;
}

void idpool_put(idpool_t *idpool, int32_t id)
{
  if(!(0 <= id && id < idpool->capacity)) error();
  if(idpool->used_flag[id] == FALSE) error();
  
  idpool->stack[idpool->sp++] = id;
  idpool->used_flag[id] = FALSE;
  idpool->size--;
  return;
}

xidpool_t* xidpool_alloc(int32_t init_capacity)
{
  xidpool_t *xidpool;
  
  xidpool = (xidpool_t*)my_malloc(sizeof(xidpool_t));
  xidpool->idpool = idpool_alloc(init_capacity);
  pthread_mutex_init(&xidpool->mutex, NULL);
  return xidpool;
}

void xidpool_free(xidpool_t *xidpool)
{
  pthread_mutex_destroy(&xidpool->mutex);
  idpool_free(xidpool->idpool);
  my_free(xidpool);
  return;
}

int32_t xidpool_isempty(xidpool_t *xidpool)
{
  int32_t ret;
  
  pthread_mutex_lock(&xidpool->mutex);
  ret = idpool_isempty(xidpool->idpool);
  pthread_mutex_unlock(&xidpool->mutex);
  return ret;
}

int32_t xidpool_size(xidpool_t *xidpool)
{
  int32_t size;
  
  pthread_mutex_lock(&xidpool->mutex);
  size = idpool_size(xidpool->idpool);
  pthread_mutex_unlock(&xidpool->mutex);
  return size;
}

int32_t xidpool_get(xidpool_t *xidpool)
{
  int32_t id;
  
  pthread_mutex_lock(&xidpool->mutex);
  id = idpool_get(xidpool->idpool);
  pthread_mutex_unlock(&xidpool->mutex);
  return id;
}

void xidpool_put(xidpool_t *xidpool, int32_t id)
{
  pthread_mutex_lock(&xidpool->mutex);
  idpool_put(xidpool->idpool, id);
  pthread_mutex_unlock(&xidpool->mutex);
  return;
}

/* lease */

lease_t* lease_alloc(int32_t init_capacity, float expand_rate, void *undef_elem)
{
  lease_t *lease;
  
  if(init_capacity == CONTAINER_DEFAULT) init_capacity = LEASE_INIT_CAPACITY;
  if(expand_rate == CONTAINER_DEFAULT) expand_rate = LEASE_EXPAND_RATE;
  
  lease = (lease_t*)my_malloc(sizeof(lease_t));
  lease->init_capacity = init_capacity;
  lease->expand_rate = expand_rate;
  lease->vector = (vector_t*)vector_alloc(init_capacity, expand_rate, undef_elem);
  lease->idpool = (idpool_t*)idpool_alloc(init_capacity);
  return lease;
}

void lease_free(lease_t *lease)
{
  vector_free(lease->vector);
  idpool_free(lease->idpool);
  my_free(lease);
  return;
}

int lease_isempty(lease_t *lease)
{
  int ret;
  
  ret = idpool_isempty(lease->idpool);
  return ret;
}

int32_t lease_size(lease_t *lease)
{
  int32_t size;
  
  size = idpool_size(lease->idpool);
  return size;
}

int32_t lease_putin(lease_t *lease, void *elem)
{
  int32_t id;
  
  id = idpool_get(lease->idpool);
  vector_assign(lease->vector, id, elem);
  return id;
}

void* lease_pickup(lease_t *lease, int32_t id)
{
  void *elem;
  
  idpool_put(lease->idpool, id);
  elem = vector_at(lease->vector, id);
  vector_assign(lease->vector, id, NULL);
  return elem;
}

void* lease_at(lease_t *lease, int32_t pos)
{
  void *elem;
  
  elem = vector_at(lease->vector, pos);
  return elem;
}

xlease_t* xlease_alloc(int32_t init_capacity, float expand_rate, void *undef_elem)
{
  xlease_t *xlease;
  
  xlease = (xlease_t*)my_malloc(sizeof(xlease_t));
  xlease->lease = lease_alloc(init_capacity, expand_rate, undef_elem);
  pthread_mutex_init(&xlease->mutex, NULL);
  return xlease;
}

void xlease_free(xlease_t *xlease)
{
  pthread_mutex_destroy(&xlease->mutex);
  lease_free(xlease->lease);
  my_free(xlease);
  return;
}

int xlease_isempty(xlease_t *xlease)
{
  int ret;
  
  pthread_mutex_lock(&xlease->mutex);
  ret = lease_isempty(xlease->lease);
  pthread_mutex_unlock(&xlease->mutex);
  return ret;
}

int32_t xlease_size(xlease_t *xlease)
{
  int32_t size;
  
  pthread_mutex_lock(&xlease->mutex);
  size = lease_size(xlease->lease);
  pthread_mutex_unlock(&xlease->mutex);
  return size;
}

int32_t xlease_putin(xlease_t *xlease, void *elem)
{
  int32_t id;
  
  pthread_mutex_lock(&xlease->mutex);
  id = lease_putin(xlease->lease, elem);
  pthread_mutex_unlock(&xlease->mutex);
  return id;
}

void*xlease_pickup(xlease_t *xlease, int32_t id)
{
  void *elem;
  
  pthread_mutex_lock(&xlease->mutex);
  elem = lease_pickup(xlease->lease, id);
  pthread_mutex_unlock(&xlease->mutex);
  return elem;
}

void* xlease_at(xlease_t *xlease, int32_t pos)
{
  void *elem;
  
  pthread_mutex_lock(&xlease->mutex);
  elem = lease_at(xlease->lease, pos);
  pthread_mutex_unlock(&xlease->mutex);
  return elem;
}

/* deque */

deque_t* deque_alloc(int32_t init_capacity, float expand_rate)
{
  deque_t *deque;
  
  if(init_capacity == CONTAINER_DEFAULT) init_capacity = DEQUE_INIT_CAPACITY;
  if(expand_rate == CONTAINER_DEFAULT) expand_rate = DEQUE_EXPAND_RATE;
  
  deque = (deque_t*)my_malloc(sizeof(deque_t));
  deque->expand_rate = expand_rate;
  deque->init_capacity = init_capacity + 1;
  deque->capacity = deque->init_capacity;
  deque->buf = (void**)my_malloc(deque->capacity * sizeof(void*));
  deque->head = 0;
  deque->tail = 0;
  return deque;
}

void deque_free(deque_t *deque)
{
  my_free(deque->buf);
  my_free(deque);
  return;
}

void deque_clear(deque_t *deque)
{
  my_free(deque->buf);
  deque->capacity = deque->init_capacity;
  deque->buf = (void**)my_malloc(deque->capacity * sizeof(void*));
  deque->head = 0;
  deque->tail = 0;
  return;
}

int32_t deque_isempty(deque_t *deque)
{
  int32_t ret;
  
  ret = deque->head == deque->tail ? TRUE : FALSE;
  return ret;
}

int32_t deque_size(deque_t *deque)
{
  int32_t size;
  
  size = (deque->tail - deque->head + deque->capacity) % deque->capacity;
  return size;
}

void* deque_front(deque_t *deque)
{
  void *elem;
  
  if(deque->head == deque->tail) error();
  
  elem = deque->buf[(deque->head + 1) % deque->capacity];
  return elem;
}

void* deque_back(deque_t *deque)
{
  void *elem;
  
  if(deque->head == deque->tail) error();
  
  elem = deque->buf[deque->tail];
  return elem;
}

void deque_push(deque_t *deque, void *elem)
{
  int32_t old_capacity;
  
  if((deque->tail + 1) % deque->capacity == deque->head)
    {
      old_capacity = deque->capacity;
      deque->capacity = old_capacity * deque->expand_rate;
      deque->buf = (void**)my_realloc(deque->buf, deque->capacity * sizeof(void*));
      memcpy(deque->buf + old_capacity, deque->buf, deque->head * sizeof(void*));
      deque->tail = old_capacity + deque->head - 1;
    }
  
  deque->tail = (deque->tail + 1) % deque->capacity;
  deque->buf[deque->tail] = elem;
  return;
}

void* deque_pop(deque_t *deque)
{
  void *elem;
  
  if(deque->head == deque->tail) error();
  
  elem = deque->buf[deque->tail];
  deque->tail = (deque->tail - 1 + deque->capacity) % deque->capacity;
  return elem;
}

void* deque_shift(deque_t *deque)
{
  void *elem;
  
  if(deque->head == deque->tail) error();
  
  elem = deque->buf[(deque->head + 1) % deque->capacity];
  deque->head = (deque->head + 1) % deque->capacity;
  return elem;
}

void deque_unshift(deque_t *deque, void *elem)
{
  int32_t old_capacity;
  
  if((deque->tail + 1) % deque->capacity == deque->head)
    {
      old_capacity = deque->capacity;
      deque->capacity = old_capacity * deque->expand_rate;
      deque->buf = (void**)my_realloc(deque->buf, deque->capacity * sizeof(void*));
      memcpy(deque->buf + old_capacity, deque->buf, deque->head * sizeof(void*));
      deque->tail = old_capacity + deque->head - 1;
    }
  
  deque->head = (deque->head - 1 + deque->capacity) % deque->capacity;
  deque->buf[(deque->head + 1) % deque->capacity] = elem;
  return;
}

void* deque_at(deque_t *deque, int32_t pos)
{
  void *elem;
  
  if(!(0 <= pos && pos < deque_size(deque))) error();
  
  elem = deque->buf[(deque->head + pos + 1) % deque->capacity];
  return elem;
}

void deque_assign(deque_t *deque, int32_t pos, void *elem)
{
  if(!(0 <= pos && pos < deque_size(deque))) error();
  
  deque->buf[(deque->head + pos + 1) % deque->capacity] = elem;
  return;
}

xdeque_t* xdeque_alloc(int32_t init_capacity, float expand_rate)
{
  xdeque_t *xdeque;
  
  xdeque = (xdeque_t*)my_malloc(sizeof(xdeque_t));
  xdeque->deque = deque_alloc(init_capacity, expand_rate);
  pthread_mutex_init(&xdeque->mutex, NULL);
  return xdeque;
}

void xdeque_free(xdeque_t *xdeque)
{
  pthread_mutex_destroy(&xdeque->mutex);
  deque_free(xdeque->deque);
  my_free(xdeque);
  return;
}

void xdeque_clear(xdeque_t *xdeque)
{
  pthread_mutex_lock(&xdeque->mutex);
  deque_clear(xdeque->deque);
  pthread_mutex_unlock(&xdeque->mutex);
  return;
}

int32_t xdeque_isempty(xdeque_t *xdeque)
{
  int32_t ret;
  
  pthread_mutex_lock(&xdeque->mutex);
  ret = deque_isempty(xdeque->deque);
  pthread_mutex_unlock(&xdeque->mutex);
  return ret;
}

int32_t xdeque_size(xdeque_t *xdeque)
{
  int32_t size;
  
  pthread_mutex_lock(&xdeque->mutex);
  size = deque_size(xdeque->deque);
  pthread_mutex_unlock(&xdeque->mutex);
  return size;
}

void* xdeque_front(xdeque_t *xdeque)
{
  void *elem;
  
  pthread_mutex_lock(&xdeque->mutex);
  elem = deque_front(xdeque->deque);
  pthread_mutex_unlock(&xdeque->mutex);
  return elem;
}

void* xdeque_back(xdeque_t *xdeque)
{
  void *elem;
  
  pthread_mutex_lock(&xdeque->mutex);
  elem = deque_back(xdeque->deque);
  pthread_mutex_unlock(&xdeque->mutex);
  return elem;
}

void xdeque_push(xdeque_t *xdeque, void *elem)
{
  pthread_mutex_lock(&xdeque->mutex);
  deque_push(xdeque->deque, elem);
  pthread_mutex_unlock(&xdeque->mutex);
  return;
}

void* xdeque_pop(xdeque_t *xdeque)
{
  void *elem;
  
  pthread_mutex_lock(&xdeque->mutex);
  elem = deque_pop(xdeque->deque);
  pthread_mutex_unlock(&xdeque->mutex);
  return elem;
}

void* xdeque_shift(xdeque_t *xdeque)
{
  void *elem;
  
  pthread_mutex_lock(&xdeque->mutex);
  elem = deque_shift(xdeque->deque);
  pthread_mutex_unlock(&xdeque->mutex);
  return elem;
}

void xdeque_unshift(xdeque_t *xdeque, void *elem)
{
  pthread_mutex_lock(&xdeque->mutex);
  deque_unshift(xdeque->deque, elem);
  pthread_mutex_unlock(&xdeque->mutex);
  return;
}

void* xdeque_at(xdeque_t *xdeque, int32_t pos)
{
  void *elem;
  
  pthread_mutex_lock(&xdeque->mutex);
  elem = deque_at(xdeque->deque, pos);
  pthread_mutex_unlock(&xdeque->mutex);
  return elem;
}

void xdeque_assign(xdeque_t *xdeque, int32_t pos, void *elem)
{
  pthread_mutex_lock(&xdeque->mutex);
  deque_assign(xdeque->deque, pos, elem);
  pthread_mutex_unlock(&xdeque->mutex);
  return;
}

/* taskque */

taskque_t* taskque_alloc(int32_t init_capacity, float expand_rate, void *notify_elem)
{
  taskque_t *taskque;
  
  taskque = (taskque_t*)my_malloc(sizeof(taskque_t));
  taskque->notify_flag = FALSE;
  taskque->deque = deque_alloc(init_capacity, expand_rate);
  taskque->notify_elem = notify_elem;
  pthread_mutex_init(&taskque->mutex, NULL);
  pthread_cond_init(&taskque->cond, NULL);
  return taskque;
}

void taskque_free(taskque_t *taskque)
{
  pthread_cond_destroy(&taskque->cond);
  pthread_mutex_destroy(&taskque->mutex);
  deque_free(taskque->deque);
  my_free(taskque);
  return;
}

int32_t taskque_isempty(taskque_t *taskque)
{
  int32_t ret;
  
  pthread_mutex_lock(&taskque->mutex);
  
  ret = deque_isempty(taskque->deque);
  
  pthread_mutex_unlock(&taskque->mutex);
  return ret;
}

int32_t taskque_size(taskque_t *taskque)
{
  int32_t size;
  
  pthread_mutex_lock(&taskque->mutex);
  
  size = deque_size(taskque->deque);
  
  pthread_mutex_unlock(&taskque->mutex);
  return size;
}

void taskque_unshift(taskque_t *taskque, void *elem)
{
  pthread_mutex_lock(&taskque->mutex);
  
  deque_unshift(taskque->deque, elem);
  
  pthread_cond_broadcast(&taskque->cond);
  pthread_mutex_unlock(&taskque->mutex);
  return;
}

void* taskque_pop(taskque_t *taskque)
{
  void *elem;
  
  pthread_mutex_lock(&taskque->mutex);
  
  while(deque_isempty(taskque->deque) == TRUE && taskque->notify_flag == FALSE)
    {
      pthread_cond_wait(&taskque->cond, &taskque->mutex);
    }
  
  if(taskque->notify_flag == TRUE)
    {
      taskque->notify_flag = FALSE;
      elem = taskque->notify_elem;
    }
  else
    {
      elem = deque_pop(taskque->deque);
    }
  
  pthread_mutex_unlock(&taskque->mutex);
  return elem;
}

void* taskque_at(taskque_t *taskque, int32_t pos)
{
  void *elem;
  
  pthread_mutex_lock(&taskque->mutex);
  
  elem = deque_at(taskque->deque, pos);
  
  pthread_mutex_unlock(&taskque->mutex);
  return elem;
}

void taskque_notify(taskque_t *taskque)
{
  pthread_mutex_lock(&taskque->mutex);
  
  taskque->notify_flag = TRUE;
  
  pthread_cond_broadcast(&taskque->cond);
  pthread_mutex_unlock(&taskque->mutex);
  return;
}

/* list */

cell_t* cell_alloc(void *elem)
{
  cell_t *cell;
  
  cell = (cell_t*)my_malloc(sizeof(cell_t));
  cell->elem = elem;
  cell->prev = NULL;
  cell->next = NULL;
  return cell;
}

void cell_free(cell_t *cell)
{
  my_free(cell);
  return;
}

list_t* list_alloc(void)
{
  list_t *list;
  
  list = (list_t*)my_malloc(sizeof(list_t));
  list->head = cell_alloc(NULL);
  list->head->prev = list->head;
  list->head->next = list->head;
  list->iter = list->head;
  list->size = 0;
  list->pos = -1;
  return list;
}

void list_free(list_t *list)
{
  list_clear(list);
  cell_free(list->head);
  my_free(list);
  return;
}

void list_clear(list_t *list)
{
  cell_t *curr_cell, *next_cell;
  
  curr_cell = list->head->next;
  while(curr_cell != list->head)
    {
      next_cell = curr_cell->next;
      cell_free(curr_cell);
      curr_cell = next_cell;
    }
  list->head->prev = list->head;
  list->head->next = list->head;
  list->iter = list->head;
  list->size = 0;
  list->pos = -1;
  return;
}

void list_head(list_t *list)
{
  list->iter = list->head;
  list->pos = -1;
  return;
}

void list_tail(list_t *list)
{
  list_head(list);
  return;
}

int32_t list_isempty(list_t *list)
{
  int32_t ret;
  
  ret = list->size == 0 ? TRUE : FALSE;
  return ret;
}

int32_t list_size(list_t *list)
{
  int32_t size;
  
  size = list->size;
  return size;
}

void* list_curr(list_t *list)
{
  void *elem;
  
  if(list->iter == list->head) error();
  
  elem = list->iter->elem;
  return elem;
}

void* list_next(list_t *list)
{
  void *elem;
  
  list->iter = list->iter->next;
  list->pos++;
  if(list->pos == list->size)
    {
      list->pos = -1;
    }
  elem = list->iter->elem;
  return elem;
}

void* list_prev(list_t *list)
{
  void *elem;
  
  list->iter = list->iter->prev;
  list->pos--;
  if(list->pos == -2)
    {
      list->pos = list->size - 1;
    }
  elem = list->iter->elem;
  return elem;
}

int32_t list_hascurr(list_t *list)
{
  int32_t ret;
  
  ret = list->iter != list->head ? TRUE : FALSE;
  return ret;
}

int32_t list_hasnext(list_t *list)
{
  int32_t ret;
  
  ret = list->iter->next != list->head ? TRUE : FALSE;
  return ret;
}

int32_t list_hasprev(list_t *list)
{
  int32_t ret;
  
  ret = list->iter->prev != list->head ? TRUE : FALSE;
  return ret;
}

void list_push(list_t *list, void *elem)
{
  cell_t *cell;
  
  cell = cell_alloc(elem);
  cell->next = list->head;
  cell->prev = list->head->prev;
  list->head->prev->next = cell;
  list->head->prev = cell;
  list->size++;
  list->iter = list->head->prev;
  list->pos = list->size - 1;
  return;
}

void* list_pop(list_t *list)
{
  cell_t *cell;
  void *elem;
  
  if(list->size <= 0) error();
  
  cell = list->head->prev;
  list->head->prev = cell->prev;
  cell->prev->next = list->head;
  elem = cell->elem;
  cell_free(cell);
  list->size--;
  list->iter = list->head->prev;
  list->pos = list->size - 1;
  return elem;
}

void* list_shift(list_t *list)
{
  cell_t *cell;
  void *elem;
  
  if(list->size <= 0) error();
  
  cell = list->head->next;
  list->head->next = cell->next;
  cell->next->prev = list->head;
  elem = cell->elem;
  cell_free(cell);
  list->size--;
  list->iter = list->head->next;
  list->pos = 0;
  return elem;
}

void list_unshift(list_t *list, void *elem)
{
  cell_t *cell;
  
  cell = cell_alloc(elem);
  cell->next = list->head->next;
  cell->prev = list->head;
  list->head->next->prev = cell;
  list->head->next = cell;
  list->size++;
  list->iter = list->head->next;
  list->pos = 0;
  return;
}

void list_insert(list_t *list, void *elem)
{
  cell_t *cell;
  
  cell = cell_alloc(elem);
  cell->next = list->iter;
  cell->prev = list->iter->prev;
  list->iter->prev->next = cell;
  list->iter->prev = cell;
  list->size++;
  list->pos++;
  return;
}

void* list_remove(list_t *list)
{
  cell_t *cell;
  void *elem;
  
  if(list->iter == list->head) error();
  
  list->iter->prev->next = list->iter->next;
  list->iter->next->prev = list->iter->prev;
  cell = list->iter;
  elem = cell->elem;
  list->iter = list->iter->next;
  cell_free(cell);
  list->size--;
  if(list->pos == list->size)
    {
      list->pos = -1;
    }
  return elem;
}

void list_move(list_t *list, int32_t pos)
{
  int i, times;
  
  if(!(0 <= pos && pos < list->size)) error();
  
  if(list->pos <= pos)
    {
      times = pos - list->pos;
      for(i = 0; i < times; i++)
        {
          list_next(list);
        }
    }
  else
    {
      times = list->pos - pos;
      for(i = 0; i < times; i++)
        {
          list_prev(list);
        }
    }
  if(list->pos != pos) error();
  return;
}

void* list_at(list_t *list, int32_t pos)
{
  void *elem;
  
  list_move(list, pos);
  elem = list->iter->elem;
  return elem;
}

void list_assign(list_t *list, int32_t pos, void *elem)
{
  list_move(list, pos);
  list->iter->elem = elem;
  return;
}

xlist_t* xlist_alloc(void)
{
  xlist_t *xlist;
  
  xlist = (xlist_t*)my_malloc(sizeof(xlist_t));
  xlist->list = list_alloc();
  pthread_mutex_init(&xlist->mutex, NULL);
  return xlist;
}

void xlist_free(xlist_t *xlist)
{
  pthread_mutex_destroy(&xlist->mutex);
  list_free(xlist->list);
  my_free(xlist);
  return;
}

void xlist_clear(xlist_t *xlist)
{
  pthread_mutex_lock(&xlist->mutex);
  list_clear(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return;
}

void xlist_head(xlist_t *xlist)
{
  pthread_mutex_lock(&xlist->mutex);
  list_head(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return;
}

void xlist_tail(xlist_t *xlist)
{
  pthread_mutex_lock(&xlist->mutex);
  list_tail(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return;
}

int32_t xlist_isempty(xlist_t *xlist)
{
  int32_t ret;
  
  pthread_mutex_lock(&xlist->mutex);
  ret = list_isempty(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return ret;
}

int32_t xlist_size(xlist_t *xlist)
{
  int32_t size;
  
  pthread_mutex_lock(&xlist->mutex);
  size = list_size(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return size;
}

void* xlist_curr(xlist_t *xlist)
{
  void *elem;
  
  pthread_mutex_lock(&xlist->mutex);
  elem = list_curr(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return elem;
}

void* xlist_next(xlist_t *xlist)
{
  void *elem;
  
  pthread_mutex_lock(&xlist->mutex);
  elem = list_next(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return elem;
}

void* xlist_prev(xlist_t *xlist)
{
  void *elem;
  
  pthread_mutex_lock(&xlist->mutex);
  elem = list_prev(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return elem;
}

int32_t xlist_hascurr(xlist_t *xlist)
{
  int32_t ret;
  
  pthread_mutex_lock(&xlist->mutex);
  ret = list_hascurr(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return ret;
}

int32_t xlist_hasnext(xlist_t *xlist)
{
  int32_t ret;
  
  pthread_mutex_lock(&xlist->mutex);
  ret = list_hasnext(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return ret;
}

int32_t xlist_hasprev(xlist_t *xlist)
{
  int32_t ret;
  
  pthread_mutex_lock(&xlist->mutex);
  ret = list_hasprev(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return ret;
}

void xlist_push(xlist_t *xlist, void *elem)
{
  pthread_mutex_lock(&xlist->mutex);
  list_push(xlist->list, elem);
  pthread_mutex_unlock(&xlist->mutex);
  return;
}

void* xlist_pop(xlist_t *xlist)
{
  void *elem;
  
  pthread_mutex_lock(&xlist->mutex);
  elem = list_pop(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return elem;
}

void* xlist_shift(xlist_t *xlist)
{
  void *elem;
  
  pthread_mutex_lock(&xlist->mutex);
  elem = list_shift(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return elem;
}

void xlist_unshift(xlist_t *xlist, void *elem)
{
  pthread_mutex_lock(&xlist->mutex);
  list_unshift(xlist->list, elem);
  pthread_mutex_unlock(&xlist->mutex);
  return;
}

void xlist_insert(xlist_t *xlist, void *elem)
{
  pthread_mutex_lock(&xlist->mutex);
  list_insert(xlist->list, elem);
  pthread_mutex_unlock(&xlist->mutex);
  return;
}

void* xlist_remove(xlist_t *xlist)
{
  void *elem;
  
  pthread_mutex_lock(&xlist->mutex);
  elem = list_remove(xlist->list);
  pthread_mutex_unlock(&xlist->mutex);
  return elem;
}

void xlist_move(xlist_t *xlist, int32_t pos)
{
  pthread_mutex_lock(&xlist->mutex);
  list_move(xlist->list, pos);
  pthread_mutex_unlock(&xlist->mutex);
  return;
}

void* xlist_at(xlist_t *xlist, int32_t pos)
{
  void *elem;
  
  pthread_mutex_lock(&xlist->mutex);
  elem = list_at(xlist->list, pos);
  pthread_mutex_unlock(&xlist->mutex);
  return elem;
}

void xlist_assign(xlist_t *xlist, int32_t pos, void *elem)
{
  pthread_mutex_lock(&xlist->mutex);
  list_assign(xlist->list, pos, elem);
  pthread_mutex_unlock(&xlist->mutex);
  return;
}
