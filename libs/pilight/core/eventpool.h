/*
	Copyright (C) 2015 - 2016 CurlyMo

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _EVENTPOOL_H_
#define _EVENTPOOL_H_

#ifdef _WIN32
	#if _WIN32_WINNT < 0x0501
		#undef _WIN32_WINNT
		#define _WIN32_WINNT 0x0501
	#endif
	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#define MSG_NOSIGNAL 0
#else
	#include <netinet/in.h>
	#include <pthread.h>
#endif
#include <signal.h>
#include <time.h>
#include <mbedtls/ssl.h>

#include "../../libuv/uv.h"
#include "eventpool_structs.h"

enum eventpool_threads_t {
	EVENTPOOL_NO_THREADS,
	EVENTPOOL_THREADED
} eventpool_threads_t;

#define EVENTPOOL_STAGE_SOCKET 						0
#define EVENTPOOL_STAGE_CONNECT 					1
#define EVENTPOOL_STAGE_CONNECTING				2
#define EVENTPOOL_STAGE_CONNECTED					3
#define EVENTPOOL_STAGE_BIND							4
#define EVENTPOOL_STAGE_LISTEN						5
#define EVENTPOOL_STAGE_LISTENING					6
#define EVENTPOOL_STAGE_READ							7
#define EVENTPOOL_STAGE_WRITE							8
#define EVENTPOOL_STAGE_DISCONNECTED			9
#define EVENTPOOL_STAGE_DISCONNECT				10

#define EVENTPOOL_TYPE_SOCKET_CLIENT			0
#define EVENTPOOL_TYPE_SOCKET_SERVER			1
#define EVENTPOOL_TYPE_SOCKET_SSL_CLIENT	2
#define EVENTPOOL_TYPE_SOCKET_SSL_SERVER	3
#define EVENTPOOL_TYPE_IO									4

#define EV_SOCKET_SUCCESS						0
#define EV_SOCKET_FAILED						1
#define EV_CONNECT_SUCCESS					2
#define EV_CONNECT_FAILED						3
#define EV_BIND_SUCCESS							4
#define EV_BIND_FAILED							5
#define EV_LISTEN_SUCCESS						6
#define EV_LISTEN_FAILED						7
#define EV_READ											8
#define EV_WRITE										9
#define EV_HIGHPRI									10
#define EV_IDLE											11
#define EV_DISCONNECTED							12
// #define EV_POLL											13

#define REASON_SEND_CODE							0
#define REASON_CONTROL_DEVICE					1
#define REASON_CODE_SENT							2
#define REASON_CODE_SEND_FAIL					3
#define REASON_CODE_SEND_SUCCESS			4
#define REASON_CODE_RECEIVED					5
#define REASON_RECEIVED_PULSETRAIN		6
#define REASON_RECEIVED_OOK						7
#define REASON_RECEIVED_API						8
#define REASON_BROADCAST							9
#define REASON_BROADCAST_CORE					10
#define REASON_FORWARD								11
#define REASON_CONFIG_UPDATE					12
#define REASON_CONFIG_UPDATED					13
#define REASON_SOCKET_RECEIVED				14
#define REASON_SOCKET_DISCONNECTED		15
#define REASON_SOCKET_CONNECTED				16
#define REASON_SOCKET_SEND						17
#define REASON_SSDP_RECEIVED					19
#define REASON_SSDP_RECEIVED_FREE			20
#define REASON_SSDP_DISCONNECTED			21
#define REASON_SSDP_CONNECTED					22
#define REASON_WEBSERVER_CONNECTED		23
#define REASON_DEVICE_ADDED						24
#define REASON_DEVICE_ADAPT						25
#define REASON_ADHOC_MODE							26
#define REASON_ADHOC_CONNECTED				27
#define REASON_ADHOC_CONFIG_RECEIVED	28
#define REASON_ADHOC_DATA_RECEIVED		29
#define REASON_ADHOC_UPDATE_RECEIVED	30
#define REASON_ADHOC_DISCONNECTED			31
#define REASON_SEND_BEGIN							32
#define REASON_SEND_END								33
#define REASON_ARP_FOUND_DEVICE				34
#define REASON_ARP_LOST_DEVICE				35
#define REASON_ARP_CHANGED_DEVICE			36
#define REASON_LOG										37
#define REASON_END										38

typedef struct threadpool_data_t {
	int reason;
	int priority;
	char name[255];
	uv_sem_t *ref;
	void *userdata;
	void *(*func)(int, void *, void *);
	void *(*done)(void *);
} threadpool_data_t;

typedef struct threadpool_tasks_t {
	unsigned long id;
	char *name;
	void *(*func)(int, void *, void *);
	void *(*done)(void *);
	uv_sem_t *ref;
	int priority;
	int reason;

	struct {
		struct timespec first;
		struct timespec second;
	}	timestamp;

	void *userdata;

	struct threadpool_tasks_t *next;
} threadpool_tasks_t;

typedef struct eventpool_listener_t {
	void *(*func)(int, void *, void *);
	void *userdata;
	void *data;
	int reason;
	struct eventpool_listener_t *next;
} eventpool_listener_t;

typedef struct iobuf_t {
  char *buf;
  ssize_t len;
  ssize_t size;
	uv_mutex_t lock;
} iobuf_t;

struct uv_custom_poll_t {
	int is_ssl;
	int is_server;
	int is_udp;
	int custom_recv;
	int doread;
	int dowrite;
	int doclose;
	int started;
	int action;
	int threadid;

	char *host;

	uv_timer_t *timer_req;
	uv_poll_t *poll_req;

	void *data;

	void (*write_cb)(uv_poll_t *);
	void (*close_cb)(uv_poll_t *);
	ssize_t (*read_cb)(uv_poll_t *, ssize_t, const char *);

	struct {
		int init;
		int handshake;
		mbedtls_ssl_context ctx;
	} ssl;

  struct iobuf_t recv_iobuf;
  struct iobuf_t send_iobuf;
} uv_custom_poll_t;

void eventpool_callback_remove(struct eventpool_listener_t *node);
void *eventpool_callback(int, void *(*)(int, void *, void *), void *);
void eventpool_trigger(int, void *(*)(void *), void *);
void eventpool_init(enum eventpool_threads_t);
int eventpool_gc(void);

void iobuf_free(struct iobuf_t *iobuf);
void iobuf_init(struct iobuf_t *iobuf, size_t initial_size);
void iobuf_remove(struct iobuf_t *, size_t);
size_t iobuf_append_remove(struct iobuf_t *from, struct iobuf_t *to);
size_t iobuf_append(struct iobuf_t *, const void *, int);

void uv_custom_poll_init(struct uv_custom_poll_t **, uv_poll_t *, void *);
void uv_custom_poll_free(struct uv_custom_poll_t *);
void uv_custom_poll_cb(uv_poll_t *, int, int);
int uv_custom_read(uv_poll_t *);
int uv_custom_write(uv_poll_t *);
int uv_custom_close(uv_poll_t *);

void uv_queue_work_s(uv_work_t *req, char *name, int main, uv_work_cb work_cb, uv_after_work_cb after_work_cb);

#endif
