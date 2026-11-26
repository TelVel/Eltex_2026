#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define QUEUE_KEY 1234
#define TEXT_SIZE 2048
#define MAX_SUBSCRIBERS 256
#define MAX_TOPICS 32
#define MAX_PUBLISHERS 256

typedef struct {
    long mtype;
    char text[TEXT_SIZE];
} Message;

typedef struct {
    pid_t pid;
    char topics[MAX_TOPICS][128];
    int topic_count;
} Subscriber;

static volatile sig_atomic_t stopping = 0;
static int queue_id = -1;
static Subscriber subscribers[MAX_SUBSCRIBERS];
static int subscriber_count = 0;
static pid_t publishers[MAX_PUBLISHERS];
static int publisher_count = 0;

static void handle_signal(int sig) {
    (void)sig;
    stopping = 1;
}

static int send_message(pid_t pid, const char *text) {
    Message msg;
    msg.mtype = (long)pid;
    snprintf(msg.text, sizeof(msg.text), "%s", text);
    return msgsnd(queue_id, &msg, strlen(msg.text) + 1, IPC_NOWAIT);
}

static void remember_publisher(pid_t pid) {
    for (int i = 0; i < publisher_count; i++)
        if (publishers[i] == pid)
            return;
    if (publisher_count < MAX_PUBLISHERS)
        publishers[publisher_count++] = pid;
}

static void remove_publisher(pid_t pid) {
    for (int i = 0; i < publisher_count; i++) {
        if (publishers[i] == pid) {
            publishers[i] = publishers[--publisher_count];
            return;
        }
    }
}

static void subscribe_topic(pid_t pid, const char *topic) {
    int index = -1;
    for (int i = 0; i < subscriber_count; i++)
        if (subscribers[i].pid == pid)
            index = i;

    if (index < 0) {
        if (subscriber_count >= MAX_SUBSCRIBERS)
            return;
        index = subscriber_count++;
        subscribers[index].pid = pid;
        subscribers[index].topic_count = 0;
    }

    Subscriber *s = &subscribers[index];
    for (int i = 0; i < s->topic_count; i++)
        if (strcmp(s->topics[i], topic) == 0)
            return;

    if (s->topic_count < MAX_TOPICS) {
        snprintf(s->topics[s->topic_count], sizeof(s->topics[0]), "%s", topic);
        s->topic_count++;
    }
}

static void unsubscribe_topic(pid_t pid, const char *topic) {
    for (int i = 0; i < subscriber_count; i++) {
        Subscriber *s = &subscribers[i];
        if (s->pid != pid)
            continue;
        for (int j = 0; j < s->topic_count; j++) {
            if (strcmp(s->topics[j], topic) == 0) {
                for (int k = j; k + 1 < s->topic_count; k++)
                    memcpy(s->topics[k], s->topics[k + 1], sizeof(s->topics[k]));
                s->topic_count--;
                break;
            }
        }
        if (s->topic_count == 0) {
            subscribers[i] = subscribers[--subscriber_count];
        }
        return;
    }
}

static void dispatch_message(const char *topic, const char *payload) {
    char outgoing[TEXT_SIZE];
    snprintf(outgoing, sizeof(outgoing), "topic=%s payload=%s", topic, payload);
    for (int i = 0; i < subscriber_count; i++) {
        Subscriber *s = &subscribers[i];
        for (int j = 0; j < s->topic_count; j++) {
            if (strcmp(s->topics[j], topic) == 0) {
                if (send_message(s->pid, outgoing) == -1 && errno == EAGAIN)
                    fprintf(stderr, "Очередь заполнена, сообщение для PID %ld пропущено\n", (long)s->pid);
                break;
            }
        }
    }
}

static void process_request(const char *text) {
    char operation[32] = {0};
    long raw_pid = 0;
    char topic[128] = {0};
    int consumed = 0;

    if (sscanf(text, "%31[^,],%ld,%127[^,]%n", operation, &raw_pid, topic, &consumed) < 2)
        return;

    pid_t pid = (pid_t)raw_pid;

    if (strcmp(operation, "subscribe") == 0 && topic[0]) {
        subscribe_topic(pid, topic);
        return;
    }

    if (strcmp(operation, "unsubscribe") == 0 && topic[0]) {
        unsubscribe_topic(pid, topic);
        return;
    }

    if (strcmp(operation, "publisher_done") == 0) {
        remove_publisher(pid);
        return;
    }

    if (strcmp(operation, "send") == 0 && topic[0]) {
        const char *payload = text + consumed;
        if (*payload == ',')
            payload++;
        remember_publisher(pid);
        dispatch_message(topic, payload);
    }
}

static int run_broker(void) {
    queue_id = msgget((key_t)QUEUE_KEY, IPC_CREAT | IPC_EXCL | 0666);
    if (queue_id == -1) {
        if (errno == EEXIST)
            fprintf(stderr, "Брокер уже запущен: очередь сообщений существует\n");
        else
            perror("msgget");
        return 1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    printf("Брокер запущен, PID=%ld\n", (long)getpid());
    fflush(stdout);

    while (!stopping) {
        Message msg;
        ssize_t n = msgrcv(queue_id, &msg, sizeof(msg.text), 1, 0);
        if (n == -1) {
            if (errno == EINTR)
                continue;
            if (errno == EIDRM || errno == EINVAL)
                break;
            perror("msgrcv");
            continue;
        }
        msg.text[sizeof(msg.text) - 1] = '\0';
        process_request(msg.text);
    }

    for (int i = 0; i < publisher_count; i++)
        kill(publishers[i], SIGINT);
    for (int i = 0; i < subscriber_count; i++)
        kill(subscribers[i].pid, SIGINT);

    if (queue_id != -1)
        msgctl(queue_id, IPC_RMID, NULL);

    printf("Брокер завершил работу\n");
    return 0;
}

static int run_publisher(const char *topic) {
    queue_id = msgget((key_t)QUEUE_KEY, 0666);
    if (queue_id == -1) {
        perror("Очередь брокера недоступна");
        return 1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    char *line = NULL;
    size_t capacity = 0;
    printf("Издатель PID=%ld, тема=%s. Введите сообщения; Ctrl+D для выхода.\n", (long)getpid(), topic);

    while (!stopping) {
        errno = 0;
        ssize_t n = getline(&line, &capacity, stdin);
        if (n < 0)
            break;
        while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r'))
            line[--n] = '\0';

        Message msg;
        msg.mtype = 1;
        snprintf(msg.text, sizeof(msg.text), "send,%ld,%s,%s", (long)getpid(), topic, line);
        if (msgsnd(queue_id, &msg, strlen(msg.text) + 1, 0) == -1) {
            if (errno == EIDRM || errno == EINVAL)
                break;
            perror("msgsnd");
            break;
        }
    }

    free(line);
    if (msgget((key_t)QUEUE_KEY, 0666) != -1) {
        Message done;
        done.mtype = 1;
        snprintf(done.text, sizeof(done.text), "publisher_done,%ld,", (long)getpid());
        msgsnd(queue_id, &done, strlen(done.text) + 1, IPC_NOWAIT);
    }
    return 0;
}

static int run_subscriber(int count, char **topics) {
    queue_id = msgget((key_t)QUEUE_KEY, 0666);
    if (queue_id == -1) {
        perror("Очередь брокера недоступна");
        return 1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    pid_t pid = getpid();
    for (int i = 0; i < count; i++) {
        Message msg;
        msg.mtype = 1;
        snprintf(msg.text, sizeof(msg.text), "subscribe,%ld,%s", (long)pid, topics[i]);
        if (msgsnd(queue_id, &msg, strlen(msg.text) + 1, 0) == -1) {
            perror("Не удалось отправить подписку");
            return 1;
        }
    }

    printf("Подписчик PID=%ld. Темы:", (long)pid);
    for (int i = 0; i < count; i++)
        printf(" %s", topics[i]);
    printf("\n");

    while (!stopping) {
        Message msg;
        ssize_t n = msgrcv(queue_id, &msg, sizeof(msg.text), (long)pid, 0);
        if (n == -1) {
            if (errno == EINTR)
                continue;
            if (errno == EIDRM || errno == EINVAL)
                break;
            perror("msgrcv");
            break;
        }
        msg.text[sizeof(msg.text) - 1] = '\0';
        printf("%s\n", msg.text);
        fflush(stdout);
    }

    if (msgget((key_t)QUEUE_KEY, 0666) != -1) {
        for (int i = 0; i < count; i++) {
            Message msg;
            msg.mtype = 1;
            snprintf(msg.text, sizeof(msg.text), "unsubscribe,%ld,%s", (long)pid, topics[i]);
            msgsnd(queue_id, &msg, strlen(msg.text) + 1, IPC_NOWAIT);
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "-b") == 0)
        return run_broker();

    if (argc >= 3 && strcmp(argv[1], "-p") == 0)
        return run_publisher(argv[2]);

    if (argc >= 3 && strcmp(argv[1], "-s") == 0)
        return run_subscriber(argc - 2, argv + 2);

    fprintf(stderr, "Использование:\n  %s -b\n  %s -p <topic>\n  %s -s <topic1> [topic2 ...]\n", argv[0], argv[0], argv[0]);
    return 1;
}
