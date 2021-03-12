/*
 * Copyright (c) 2021 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Atomic operation intergation test
 */
#include <ztest.h>
#include <sys/atomic.h>

#define TEST_CYCLE 10000

#define THREADS_NUM 10

#define STACK_SIZE (256 + CONFIG_TEST_EXTRA_STACKSIZE)

static K_THREAD_STACK_ARRAY_DEFINE(stack, THREADS_NUM, STACK_SIZE);

static struct k_thread thread[THREADS_NUM];

atomic_t total_atomic;

void atomic_handler(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < TEST_CYCLE; i++) {
		atomic_inc(&total_atomic);
	}
}

/**
 * @brief Verify atomic operation with threads
 *
 * @details Check if  the atomic operation go correct when multiple
 * threads access the same atomic value.
 *
 * @ingroup kernel_common_tests
 */
void test_threads_access_atomic(void)
{
	k_tid_t tid[THREADS_NUM];

	k_sched_time_slice_set(1, 10);

	for (int i = 0; i < THREADS_NUM; i++) {
		tid[i] = k_thread_create(&thread[i], stack[i], STACK_SIZE,
				atomic_handler, NULL, NULL, NULL,
				K_PRIO_PREEMPT(10), 0, K_NO_WAIT);
	}

	for (int i = 0; i < THREADS_NUM; i++) {
		k_thread_join(tid[i], K_FOREVER);
	}

	k_sched_time_slice_set(0, 10);

	printk("total_atomic = %d\n", total_atomic);
	zassert_true(total_atomic == (TEST_CYCLE * THREADS_NUM), "NULL");
}

void test_main(void)
{
	ztest_test_suite(atomic_threads,
			ztest_1cpu_unit_test(test_threads_access_atomic));
	ztest_run_test_suite(atomic_threads);
}
