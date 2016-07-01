/*
 * Copyright (C) 2016 Red Hat, Inc.  All rights reserved.
 *
 * Authors: Fabio M. Di Nitto <fabbione@kronosnet.org>
 *
 * This software licensed under GPL-2.0+, LGPL-2.0+
 */

#include "config.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libknet.h"

#include "internals.h"
#include "link.h"
#include "netutils.h"
#include "test-common.h"

static void test(void)
{
	knet_handle_t knet_h;
	int logfds[2];
	struct sockaddr_storage src, dst, get_src, get_dst;
	//struct knet_link_status link_status;
	uint8_t dynamic = 0;

	memset(&src, 0, sizeof(struct sockaddr_storage));

	if (strtoaddr("127.0.0.1", "50000", (struct sockaddr *)&src, sizeof(struct sockaddr_storage)) < 0) {
		printf("Unable to convert src to sockaddr: %s\n", strerror(errno));
		exit(FAIL);
	}

	memset(&dst, 0, sizeof(struct sockaddr_storage));

	if (strtoaddr("127.0.0.1", "50001", (struct sockaddr *)&dst, sizeof(struct sockaddr_storage)) < 0) {
		printf("Unable to convert dst to sockaddr: %s\n", strerror(errno));
		exit(FAIL);
	}

	printf("Test knet_link_get_config incorrect knet_h\n");

	memset(&get_src, 0, sizeof(struct sockaddr_storage));
	memset(&get_dst, 0, sizeof(struct sockaddr_storage));

	if ((!knet_link_get_config(NULL, 1, 0, &get_src, &get_dst, &dynamic)) || (errno != EINVAL)) {
		printf("knet_link_get_config accepted invalid knet_h or returned incorrect error: %s\n", strerror(errno));
		exit(FAIL);
	}

	setup_logpipes(logfds);

	knet_h = knet_handle_new(1, logfds[1], KNET_LOG_DEBUG);

	if (!knet_h) {
		printf("knet_handle_new failed: %s\n", strerror(errno));
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	printf("Test knet_link_get_config with unconfigured host_id\n");

	memset(&get_src, 0, sizeof(struct sockaddr_storage));
	memset(&get_dst, 0, sizeof(struct sockaddr_storage));

	if ((!knet_link_get_config(knet_h, 1, 0, &get_src, &get_dst, &dynamic)) || (errno != EINVAL)) {
		printf("knet_link_get_config accepted invalid host_id or returned incorrect error: %s\n", strerror(errno));
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	flush_logs(logfds[0], stdout);

	printf("Test knet_link_get_config with incorrect linkid\n");

	if (knet_host_add(knet_h, 1) < 0) {
		printf("Unable to add host_id 1: %s\n", strerror(errno));
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	memset(&get_src, 0, sizeof(struct sockaddr_storage));
	memset(&get_dst, 0, sizeof(struct sockaddr_storage));

	if ((!knet_link_get_config(knet_h, 1, KNET_MAX_LINK, &get_src, &get_dst, &dynamic)) || (errno != EINVAL)) {
		printf("knet_link_get_config accepted invalid linkid or returned incorrect error: %s\n", strerror(errno));
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	flush_logs(logfds[0], stdout);

	printf("Test knet_link_get_config with incorrect src_addr\n");

	memset(&get_src, 0, sizeof(struct sockaddr_storage));
	memset(&get_dst, 0, sizeof(struct sockaddr_storage));

	if ((!knet_link_get_config(knet_h, 1, 0, NULL, &get_dst, &dynamic)) || (errno != EINVAL)) {
		printf("knet_link_get_config accepted invalid src_addr or returned incorrect error: %s\n", strerror(errno));
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	flush_logs(logfds[0], stdout);

	printf("Test knet_link_get_config with incorrect dynamic\n");

	memset(&get_src, 0, sizeof(struct sockaddr_storage));
	memset(&get_dst, 0, sizeof(struct sockaddr_storage));

	if ((!knet_link_get_config(knet_h, 1, 0, &get_src, &get_dst, NULL)) || (errno != EINVAL)) {
		printf("knet_link_get_config accepted invalid dynamic or returned incorrect error: %s\n", strerror(errno));
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	flush_logs(logfds[0], stdout);

	printf("Test knet_link_get_config with unconfigured link\n");

	memset(&get_src, 0, sizeof(struct sockaddr_storage));
	memset(&get_dst, 0, sizeof(struct sockaddr_storage));

	if ((!knet_link_get_config(knet_h, 1, 0, &get_src, &get_dst, &dynamic)) || (errno != EINVAL)) {
		printf("knet_link_get_config accepted unconfigured link or returned incorrect error: %s\n", strerror(errno));
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	flush_logs(logfds[0], stdout);

	printf("Test knet_link_get_config with incorrect dst_addr\n");

	if (knet_link_set_config(knet_h, 1, 0, &src, &dst) < 0) {
		printf("Unable to configure link: %s\n", strerror(errno));
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	memset(&get_src, 0, sizeof(struct sockaddr_storage));
	memset(&get_dst, 0, sizeof(struct sockaddr_storage));

	if ((!knet_link_get_config(knet_h, 1, 0, &get_src, NULL, &dynamic)) || (errno != EINVAL)) {
		printf("knet_link_get_config accepted invalid dst_addr or returned incorrect error: %s\n", strerror(errno));
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	if (dynamic) {
		printf("knet_link_get_config returned invalid dynamic status\n");
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	flush_logs(logfds[0], stdout);

	printf("Test knet_link_get_config with correct parameters for static link\n");

	memset(&get_src, 0, sizeof(struct sockaddr_storage));
	memset(&get_dst, 0, sizeof(struct sockaddr_storage));

	if (knet_link_get_config(knet_h, 1, 0, &get_src, &get_dst, &dynamic) < 0) {
		printf("knet_link_get_config failed: %s\n", strerror(errno));
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	if ((dynamic) ||
	    (memcmp(&src, &get_src, sizeof(struct sockaddr_storage))) ||
	    (memcmp(&dst, &get_dst, sizeof(struct sockaddr_storage)))) {
		printf("knet_link_get_config returned invalid data\n");
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	flush_logs(logfds[0], stdout);

	printf("Test knet_link_get_config with correct parameters for dynamic link\n");

	if (knet_link_set_config(knet_h, 1, 0, &src, NULL) < 0) {
		printf("Unable to configure link: %s\n", strerror(errno));
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	memset(&get_src, 0, sizeof(struct sockaddr_storage));
	memset(&get_dst, 0, sizeof(struct sockaddr_storage));

	if (knet_link_get_config(knet_h, 1, 0, &get_src, &get_dst, &dynamic) < 0) {
		printf("knet_link_get_config failed: %s\n", strerror(errno));
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	if ((!dynamic) ||
	    (memcmp(&src, &get_src, sizeof(struct sockaddr_storage)))) {
		printf("knet_link_get_config returned invalid data\n");
		knet_host_remove(knet_h, 1);
		knet_handle_free(knet_h);
		flush_logs(logfds[0], stdout);
		close_logpipes(logfds);
		exit(FAIL);
	}

	flush_logs(logfds[0], stdout);

	knet_host_remove(knet_h, 1);
	knet_handle_free(knet_h);
	flush_logs(logfds[0], stdout);
	close_logpipes(logfds);
}

int main(int argc, char *argv[])
{
	need_root();

	test();

	return PASS;
}