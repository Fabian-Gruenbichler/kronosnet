#ifndef __VTY_H__
#define __VTY_H__

#define KNET_VTY_DEFAULT_PORT		50000
#define KNET_VTY_DEFAULT_MAX_CONN	4

int knet_vty_init_listener(const char *address, const unsigned short port);
void knet_vty_close_listener(int listener_fd);

void knet_vty_set_max_connections(const int max_connections);
int knet_vty_accept_connections(const int sockfd);

#endif