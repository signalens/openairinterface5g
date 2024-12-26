/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#ifndef TASK_ANSWER_THREAD_POOL_H
#define TASK_ANSWER_THREAD_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
#include <stdalign.h>
#include <stdatomic.h>
#else
#include <atomic>
#define _Atomic(X) std::atomic<X>
#define _Alignas(X) alignas(X)
#endif

#include <stddef.h>
#include <stdint.h>

#if defined(__i386__) || defined(__x86_64__)
#define LEVEL1_DCACHE_LINESIZE 64
#elif defined(__aarch64__)
// This is not always true for ARM
// in linux, you can obtain the size at runtime using sysconf (_SC_LEVEL1_DCACHE_LINESIZE)
// or from the bash with the command $ getconf LEVEL1_DCACHE_LINESIZE
// in c++ using std::hardware_destructive_interference_size
#define LEVEL1_DCACHE_LINESIZE 64
#else
#error Unknown CPU architecture
#endif

typedef struct {
  // Avoid false sharing
  _Alignas(LEVEL1_DCACHE_LINESIZE) _Atomic(int) status;
} task_ans_t;

typedef struct {
  uint8_t* buf;
  size_t len;
  size_t cap; // capacity
  task_ans_t* ans;
} thread_info_tm_t;


void join_task_ans(task_ans_t* arr, size_t len);

void completed_task_ans(task_ans_t* task);

#ifdef __cplusplus
}
#endif

#endif
