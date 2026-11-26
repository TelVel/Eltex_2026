#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SHM_NAME "/m3_task5_memory"
#define SEM_NAME "/m3_task5_mutex"
#define BLOCKS 256
#define VALUES 24

typedef struct {
	int count;
	int next;
	int values[VALUES];
} Block;

typedef struct {
	int block_count;
	int generation_done;
	int finished;
	Block blocks[BLOCKS];
} Shared;

static volatile sig_atomic_t stop = 0;

static void on_signal(int s) {
	(void)s;
	stop = 1;
}

static void signals(void) {
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = on_signal;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}

static int lock_sem(sem_t *sem) {
	while (sem_wait(sem) == -1) {
		if (errno == EINTR && !stop)
			continue;
		return -1;
	}
	return 0;
}

static int producer(void) {
	int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0660);

	if (fd == -1) {
		perror("shm_open");
		return 1;
	}

	if (ftruncate(fd, sizeof(Shared)) == -1) {
		perror("ftruncate");
		close(fd);
		shm_unlink(SHM_NAME);
		return 1;
	}

	Shared *sh = mmap(NULL, sizeof(Shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (sh == MAP_FAILED) {
		perror("mmap");
		close(fd);
		shm_unlink(SHM_NAME);
		return 1;
	}

	sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0660, 1);

	if (sem == SEM_FAILED) {
		perror("sem_open");
		munmap(sh, sizeof(Shared));
		close(fd);
		shm_unlink(SHM_NAME);
		return 1;
	}

	memset(sh, 0, sizeof(*sh));
	signals();
	srand((unsigned)time(NULL) ^ (unsigned)getpid());
	printf("Производитель POSIX PID=%ld\n", (long)getpid());

	while (!stop) {
		if (lock_sem(sem) == -1)
			break;

		if (sh->block_count == BLOCKS) {
			sh->generation_done = 1;
			sem_post(sem);
			break;
		}

		int i = sh->block_count++;
		Block *b = &sh->blocks[i];

		b->count = 1 + rand() % VALUES;
		b->next = i + 1 < BLOCKS ? i + 1 : -1;

		for (int j = 0; j < b->count; j++)
			b->values[j] = rand() % 2001 - 1000;

		sem_post(sem);
		printf("Создан набор %d, элементов: %d\n", i + 1, b->count);
		fflush(stdout);
		usleep(120000);
	}

	if (lock_sem(sem) == 0) {
		sh->generation_done = 1;
		sem_post(sem);
	}

	puts("Генерация завершена. Ожидание потребителей...");

	while (!stop) {
		int done = 1;

		if (lock_sem(sem) == -1)
			break;

		for (int i = 0; i < sh->block_count; i++)
			if (sh->blocks[i].count != 0) {
				done = 0;
				break;
			}

		if (done)
			sh->finished = 1;

		sem_post(sem);

		if (done)
			break;

		usleep(250000);
	}

	if (stop && lock_sem(sem) == 0) {
		sh->finished = 1;
		sem_post(sem);
	}

	munmap(sh, sizeof(Shared));
	close(fd);
	sem_close(sem);
	shm_unlink(SHM_NAME);
	sem_unlink(SEM_NAME);
	puts("Производитель завершил работу");
	return 0;
}

static int consumer(void) {
	int fd = shm_open(SHM_NAME, O_RDWR, 0660);

	if (fd == -1) {
		perror("shm_open: сначала запустите producer");
		return 1;
	}

	Shared *sh = mmap(NULL, sizeof(Shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (sh == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 1;
	}

	sem_t *sem = sem_open(SEM_NAME, 0);

	if (sem == SEM_FAILED) {
		perror("sem_open");
		munmap(sh, sizeof(Shared));
		close(fd);
		return 1;
	}

	signals();
	printf("Потребитель POSIX PID=%ld\n", (long)getpid());

	while (!stop) {
		int got = 0;
		int finished = 0;

		if (lock_sem(sem) == -1)
			break;

		for (int i = 0; i < sh->block_count; i++) {
			Block *b = &sh->blocks[i];

			if (b->count > 0) {
				int min = b->values[0];
				int max = b->values[0];
				int n = b->count;

				for (int j = 1; j < n; j++) {
					if (b->values[j] < min)
						min = b->values[j];
					if (b->values[j] > max)
						max = b->values[j];
				}

				b->count = 0;
				printf("PID=%ld: набор %d, min=%d, max=%d\n", (long)getpid(), i + 1, min, max);
				fflush(stdout);
				got = 1;
				break;
			}
		}

		finished = sh->finished;
		sem_post(sem);

		if (finished)
			break;

		usleep(got ? 220000 : 120000);
	}

	sem_close(sem);
	munmap(sh, sizeof(Shared));
	close(fd);
	printf("Потребитель %ld завершил работу\n", (long)getpid());
	return 0;
}

int main(int argc, char **argv) {
	if (argc != 2 || (strcmp(argv[1], "producer") && strcmp(argv[1], "consumer"))) {
		fprintf(stderr, "Использование: %s producer|consumer\n", argv[0]);
		return 1;
	}

	return strcmp(argv[1], "producer") == 0 ? producer() : consumer();
}