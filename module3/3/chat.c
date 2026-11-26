#define _POSIX_C_SOURCE 200809L
#include <mqueue.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MESSAGE_SIZE 1024
#define EXIT_PRIORITY 9
#define NORMAL_PRIORITY 1

static volatile sig_atomic_t stopping = 0;

static void handle_signal(int sig) {
    (void)sig;
    stopping = 1;
}

static int make_queue_name(char *dest, size_t size, const char *base, const char *suffix) {
    char normalized[200];
    size_t j = 0;

    for (size_t i = 0; base[i] && j < sizeof(normalized) - 1; i++) {
        unsigned char c = (unsigned char)base[i];
        if (c == '/')
            continue;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-')
            normalized[j++] = (char)c;
        else
            normalized[j++] = '_';
    }
    normalized[j] = '\0';

    if (j == 0)
        return -1;

    int n = snprintf(dest, size, "/%s%s", normalized, suffix);
    return n > 0 && (size_t)n < size ? 0 : -1;
}

static int open_queues(const char *name1, const char *name2, mqd_t *input, mqd_t *output, int *owner) {
    struct mq_attr attr;
    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MESSAGE_SIZE;

    mqd_t q1 = mq_open(name1, O_CREAT | O_EXCL | O_RDWR | O_NONBLOCK, 0660, &attr);
    if (q1 != (mqd_t)-1) {
        mqd_t q2 = mq_open(name2, O_CREAT | O_EXCL | O_RDWR | O_NONBLOCK, 0660, &attr);
        if (q2 == (mqd_t)-1) {
            int saved = errno;
            mq_close(q1);
            mq_unlink(name1);
            errno = saved;
            return -1;
        }
        *input = q1;
        *output = q2;
        *owner = 1;
        return 0;
    }

    if (errno != EEXIST)
        return -1;

    q1 = mq_open(name1, O_RDWR | O_NONBLOCK);
    if (q1 == (mqd_t)-1)
        return -1;
    mqd_t q2 = mq_open(name2, O_RDWR | O_NONBLOCK);
    if (q2 == (mqd_t)-1) {
        int saved = errno;
        mq_close(q1);
        errno = saved;
        return -1;
    }

    *input = q2;
    *output = q1;
    *owner = 0;
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_очередей>\n", argv[0]);
        return 1;
    }

    char name1[256], name2[256];
    if (make_queue_name(name1, sizeof(name1), argv[1], "_1") == -1 ||
        make_queue_name(name2, sizeof(name2), argv[1], "_2") == -1) {
        fprintf(stderr, "Некорректное имя очереди\n");
        return 1;
    }

    mqd_t input, output;
    int owner = 0;
    if (open_queues(name1, name2, &input, &output, &owner) == -1) {
        perror("Не удалось открыть очереди");
        return 1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    printf("P2P-чат запущен. Очереди: %s и %s. Для выхода используйте /exit или Ctrl+C.\n", name1, name2);
    printf("Роль: %s\n", owner ? "создатель очередей" : "подключившийся участник");
    fflush(stdout);

    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    char input_line[MESSAGE_SIZE];

    while (!stopping) {
        pfd.revents = 0;
        int ready = poll(&pfd, 1, 100);
        if (ready == -1 && errno != EINTR) {
            perror("poll");
            break;
        }

        if (ready > 0 && (pfd.revents & (POLLIN | POLLHUP))) {
            if (!fgets(input_line, sizeof(input_line), stdin)) {
                stopping = 1;
                break;
            }

            input_line[strcspn(input_line, "\r\n")] = '\0';
            if (strcmp(input_line, "/exit") == 0) {
                stopping = 1;
                break;
            }

            if (mq_send(output, input_line, strlen(input_line) + 1, NORMAL_PRIORITY) == -1) {
                if (errno == EAGAIN)
                    fprintf(stderr, "Очередь отправки заполнена\n");
                else if (errno == EBADF || errno == EINVAL)
                    stopping = 1;
                else
                    perror("mq_send");
            }
        }

        unsigned int priority;
        ssize_t n;
        do {
            n = mq_receive(input, input_line, sizeof(input_line), &priority);
        } while (n == -1 && errno == EINTR && !stopping);

        if (n >= 0) {
            if (priority == EXIT_PRIORITY) {
                printf("Собеседник завершил работу\n");
                stopping = 1;
            } else {
                input_line[n < MESSAGE_SIZE ? n : MESSAGE_SIZE - 1] = '\0';
                printf("Собеседник: %s\n", input_line);
                fflush(stdout);
            }
        } else if (errno != EAGAIN && errno != EINTR && errno != EBADF && errno != EINVAL) {
            perror("mq_receive");
        }
    }

    mq_send(output, "CHAT_FINISHED", sizeof("CHAT_FINISHED"), EXIT_PRIORITY);

    mq_close(input);
    mq_close(output);

    if (owner) {
        mq_unlink(name1);
        mq_unlink(name2);
    }

    printf("Чат завершен\n");
    return 0;
}