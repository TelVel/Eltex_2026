#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_PORT 39006
#define TEXT_SIZE 900

typedef struct {
	char type[12];
	char nickname[32];
	char text[TEXT_SIZE];
} Packet;

static volatile sig_atomic_t stop = 0;
static int sockfd = -1;
static struct sockaddr_in broadcast_addr;

static void on_signal(int s) {
	(void)s;
	stop = 1;
}

static void broadcast_packet(const char *type, const char *nick, const char *text) {
	Packet p;

	memset(&p, 0, sizeof(p));
	snprintf(p.type, sizeof(p.type), "%s", type);
	snprintf(p.nickname, sizeof(p.nickname), "%s", nick);
	snprintf(p.text, sizeof(p.text), "%s", text);

	if (sendto(sockfd, &p, sizeof(p), 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) == -1)
		perror("sendto");
}

int main(int argc, char **argv) {
	int port = DEFAULT_PORT;

	if (argc > 2) {
		fprintf(stderr, "Использование: %s [port]\n", argv[0]);
		return 1;
	}

	if (argc == 2) {
		char *end;
		long p = strtol(argv[1], &end, 10);

		if (*end || p < 1024 || p > 65535) {
			fprintf(stderr, "Некорректный порт\n");
			return 1;
		}

		port = (int)p;
	}

	char nick[32];

	snprintf(nick, sizeof(nick), "user-%ld", (long)getpid());
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd == -1) {
		perror("socket");
		return 1;
	}

	int yes = 1;

	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)) == -1 ||
	    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("setsockopt");
		close(sockfd);
		return 1;
	}

	struct sockaddr_in local;

	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons((uint16_t)port);

	if (bind(sockfd, (struct sockaddr *)&local, sizeof(local)) == -1) {
		perror("bind");
		close(sockfd);
		return 1;
	}

	memset(&broadcast_addr, 0, sizeof(broadcast_addr));
	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_port = htons((uint16_t)port);
	broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = on_signal;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	printf("UDP-чат: %s, порт %d. Для выхода введите /exit.\n", nick, port);
	broadcast_packet("JOIN", nick, "подключился к чату");

	while (!stop) {
		fd_set set;

		FD_ZERO(&set);
		FD_SET(sockfd, &set);
		FD_SET(STDIN_FILENO, &set);

		int maxfd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;
		int r = select(maxfd + 1, &set, NULL, NULL, NULL);

		if (r == -1) {
			if (errno == EINTR)
				continue;
			perror("select");
			break;
		}

		if (FD_ISSET(sockfd, &set)) {
			Packet p;
			struct sockaddr_in from;
			socklen_t len = sizeof(from);
			ssize_t n = recvfrom(sockfd, &p, sizeof(p), 0, (struct sockaddr *)&from, &len);

			if (n >= 0) {
				p.type[sizeof(p.type)-1] = '\0';
				p.nickname[sizeof(p.nickname)-1] = '\0';
				p.text[sizeof(p.text)-1] = '\0';

				if (strcmp(p.nickname, nick) != 0) {
					char ip[INET_ADDRSTRLEN];

					inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));

					if (!strcmp(p.type, "JOIN"))
						printf("%s %s (%s)\n", p.nickname, p.text, ip);
					else if (!strcmp(p.type, "LEAVE"))
						printf("%s покинул чат (%s)\n", p.nickname, ip);
					else if (!strcmp(p.type, "CHAT"))
						printf("%s: %s\n", p.nickname, p.text);

					fflush(stdout);
				}
			}
		}

		if (FD_ISSET(STDIN_FILENO, &set)) {
			char line[TEXT_SIZE];

			if (!fgets(line, sizeof(line), stdin)) {
				stop = 1;
				break;
			}

			line[strcspn(line, "\r\n")] = '\0';

			if (!strcmp(line, "/exit")) {
				stop = 1;
				break;
			}

			if (line[0])
				broadcast_packet("CHAT", nick, line);
		}
	}

	broadcast_packet("LEAVE", nick, "отключился от чата");
	usleep(50000);
	close(sockfd);
	puts("Чат завершен");
	return 0;
}