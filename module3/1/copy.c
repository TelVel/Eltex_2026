#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define BLOCK 4096
#define NAME  4096

enum { READY = 1, META = 2, DATA = 3, END = 4, SKIP = 5 };

struct packet { 
    int type; 
    uint32_t len; 
    uint64_t size; 
    char name[NAME]; };

static int io(int fd, void *b, size_t n, int w)
{
    char *p = b;
    while (n) {
        ssize_t r = w ? write(fd, p, n) : read(fd, p, n);
        if (r < 0) {
            if (errno == EINTR) continue; 
            return -1; 
        }
        if (!r) return -1;
        p += r; n -= (size_t)r;
    }
    return 0;
}

static int put(int fd, int t, uint32_t l, uint64_t s, const char *nm)
{
    struct packet p;
    memset(&p, 0, sizeof p);
    p.type = t; 
    p.len = l; 
    p.size = s;
    if (nm) strncpy(p.name, nm, NAME - 1);
    return io(fd, &p, sizeof p, 1);
}

static int child(int in, int out)
{
    char buf[BLOCK];
    for (;;) {
        if (put(out, READY, 0, 0, NULL) < 0) return 1;
        struct packet p;
        if (io(in, &p, sizeof p, 0) < 0) return 1;
        if (p.type == END) return 0;
        if (p.type == SKIP) continue;
        if (p.type != META) return 1;

        char outn[NAME + 8];
        snprintf(outn, sizeof outn, "%s.copy", p.name);
        int fd = open(outn, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd < 0) perror(outn);

        uint64_t rem = p.size;
        while (rem) {
            struct packet d;
            if (io(in, &d, sizeof d, 0) < 0) {
                if (fd >= 0) close(fd);
                return 1;
            }
            if (d.type != DATA || d.len > BLOCK || d.len > rem) {
                if (fd >= 0) close(fd);
                return 1;
            }
            if (io(in, buf, d.len, 0) < 0) {
                if (fd >= 0) close(fd);
                return 1;
            }
            if (fd >= 0 && io(fd, buf, d.len, 1) < 0) { 
                perror(outn); 
                close(fd); 
                fd = -1; 
            }
            rem -= d.len;
        }
        if (fd >= 0) close(fd);
    }
}

static int parent(int out, int in, char **files, int n)
{
    char buf[BLOCK];
    for (int i = 0; i < n; i++) {
        struct packet p;
        if (io(in, &p, sizeof p, 0) < 0 || p.type != READY) return 1;

        int fd = open(files[i], O_RDONLY);
        struct stat st;
        if (fd < 0 || fstat(fd, &st) < 0) {
            perror(files[i]);
            if (fd >= 0) close(fd);
            if (put(out, SKIP, 0, 0, NULL) < 0) return 1;
            continue;
        }
        if (put(out, META, 0, (uint64_t)st.st_size, files[i]) < 0) { 
            close(fd); 
            return 1; 
        }

        uint64_t rem = (uint64_t)st.st_size;
        while (rem) {
            size_t want = rem > BLOCK ? BLOCK : (size_t)rem;
            ssize_t r = read(fd, buf, want);
            if (r < 0) { 
                if (errno == EINTR) continue; 
                perror(files[i]); 
                close(fd); 
                return 1; 
            }
            if (!r) break;
            if (put(out, DATA, (uint32_t)r, 0, NULL) < 0 || io(out, buf, (size_t)r, 1) < 0) { 
                close(fd); 
                return 1; 
            }
            rem -= (uint64_t)r;
        }
        close(fd);
        if (rem) return 1;
    }
    struct packet p;
    if (io(in, &p, sizeof p, 0) < 0 || p.type != READY) return 1;
    return put(out, END, 0, 0, NULL);
}

int main(int argc, char **argv)
{
    char *fifo = NULL;
    char **files = malloc(sizeof(char*) * (size_t)argc);
    int n = 0;
    if (!files) { perror("malloc"); return 1; }

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-p")) {
            if (i + 1 >= argc) { 
                fprintf(stderr, "-p requires argument\n"); 
                free(files); 
                return 1; 
            }
            fifo = argv[++i];
        } else files[n++] = argv[i];
    }
    if (!n) { 
        fprintf(stderr, "no input files\n"); 
        free(files); 
        return 1; 
    }

    int ctl[2], dp[2] = { -1, -1 };
    if (pipe(ctl) < 0) { 
        perror("pipe"); 
        free(files); 
        return 1; 
    }

    if (!fifo) {
        if (pipe(dp) < 0) { 
            perror("pipe"); 
            close(ctl[0]); 
            close(ctl[1]); 
            free(files); 
            return 1; 
        }
    } else if (mkfifo(fifo, 0666) < 0 && errno != EEXIST) {
        perror("mkfifo"); 
        close(ctl[0]); 
        close(ctl[1]); 
        free(files); 
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) { 
        perror("fork"); 
        free(files); return 1; 
    }

    if (pid == 0) {
        close(ctl[0]);
        int in;
        if (!fifo) { 
            close(dp[1]);
            in = dp[0]; 
        }
        else if ((in = open(fifo, O_RDONLY)) < 0) { 
            perror("open fifo"); 
            _exit(1); 
        }
        int r = child(in, ctl[1]);
        close(in); 
        close(ctl[1]);
        _exit(r);
    }

    close(ctl[1]);
    int out;
    if (!fifo) { 
        close(dp[0]); 
        out = dp[1]; 
    }
    else if ((out = open(fifo, O_WRONLY)) < 0) {
        perror("open fifo"); 
        close(ctl[0]);
        kill(pid, SIGTERM); 
        waitpid(pid, NULL, 0);
        unlink(fifo); 
        free(files); 
        return 1;
    }

    int r = parent(out, ctl[0], files, n);
    close(out); 
    close(ctl[0]);
    int st; 
    waitpid(pid, &st, 0);
    if (fifo) unlink(fifo);
    free(files);
    return r;
}