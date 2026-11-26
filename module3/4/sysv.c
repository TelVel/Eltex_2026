#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define KEY_PATH  "/tmp"
#define SHM_PROJ  'S'
#define SEM_PROJ  'M'

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

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

static volatile sig_atomic_t stop = 0;

static void on_signal(int s) { (void)s; stop = 1; }

static int lock_sem(int id) {
	struct sembuf op = {0, -1, SEM_UNDO};
	while (semop(id, &op, 1) == -1) {
		if (errno == EINTR && !stop)
			continue;
		return -1;
	}
	return 0;
}

static int unlock_sem(int id) {
	struct sembuf op = {0, 1, SEM_UNDO};
	while (semop(id, &op, 1) == -1) {
		if (errno == EINTR)
			continue;
		perror("semop (unlock)");
		return -1;
	}
	return 0;
}

static void signals(void) {
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = on_signal;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT,  &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}

static int producer(void) {
	key_t shm_key = ftok(KEY_PATH, SHM_PROJ);
	key_t sem_key = ftok(KEY_PATH, SEM_PROJ);

	if (shm_key == (key_t)-1 || sem_key == (key_t)-1) {
		perror("ftok");
		return 1;
	}

	int shmid = shmget(shm_key, sizeof(Shared), IPC_CREAT | IPC_EXCL | 0660);
	if (shmid == -1) { perror("shmget"); return 1; }

	int semid = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0660);
	if (semid == -1) {
		perror("semget");
		shmctl(shmid, IPC_RMID, NULL);
		return 1;
	}

	union semun arg;
	arg.val = 1;
	if (semctl(semid, 0, SETVAL, arg) == -1) {
		perror("semctl");
		semctl(semid, 0, IPC_RMID);
		shmctl(shmid, IPC_RMID, NULL);
		return 1;
	}

	Shared *sh = shmat(shmid, NULL, 0);
	if (sh == (void *)-1) {
		perror("shmat");
		semctl(semid, 0, IPC_RMID);
		shmctl(shmid, IPC_RMID, NULL);
		return 1;
	}

	memset(sh, 0, sizeof(*sh));
	signals();
	srand((unsigned)time(NULL) ^ (unsigned)getpid());
	printf("Производитель System V PID=%ld\n", (long)getpid());

	while (!stop) {
		if (lock_sem(semid) == -1)
			break;

		if (sh->block_count == BLOCKS) {
			sh->generation_done = 1;
			unlock_sem(semid);
			break;
		}

		int i = sh->block_count++;
		Block *b = &sh->blocks[i];
		b->count = 1 + rand() % VALUES;
		b->next  = (i + 1 < BLOCKS) ? i + 1 : -1;

		for (int j = 0; j < b->count; j++)
			b->values[j] = rand() % 2001 - 1000;

		unlock_sem(semid);
		printf("Создан набор %d, элементов: %d\n", i + 1, b->count);
		fflush(stdout);
		usleep(120000);
	}

	if (lock_sem(semid) == 0) {
		sh->generation_done = 1;
		unlock_sem(semid);
	}

	puts("Генерация завершена. Ожидание потребителей...");

	while (!stop) {
		int done = 1;

		if (lock_sem(semid) == -1)
			break;

		for (int i = 0; i < sh->block_count; i++)
			if (sh->blocks[i].count != 0) { done = 0; break; }

		if (done) sh->finished = 1;
		unlock_sem(semid);
		if (done) break;

		usleep(250000);
	}

	if (stop && lock_sem(semid) == 0) {
		sh->finished = 1;
		unlock_sem(semid);
	}

	shmdt(sh);
	shmctl(shmid, IPC_RMID, NULL);
	semctl(semid, 0, IPC_RMID);
	puts("Производитель завершил работу");
	return 0;
}

static int consumer(void) {
	key_t shm_key = ftok(KEY_PATH, SHM_PROJ);
	key_t sem_key = ftok(KEY_PATH, SEM_PROJ);

	if (shm_key == (key_t)-1 || sem_key == (key_t)-1) {
		perror("ftok");
		return 1;
	}

	int shmid = shmget(shm_key, sizeof(Shared), 0660);
	if (shmid == -1) { perror("shmget: сначала запустите producer"); return 1; }

	int semid = semget(sem_key, 1, 0660);
	if (semid == -1) { perror("semget"); return 1; }

	Shared *sh = shmat(shmid, NULL, 0);
	if (sh == (void *)-1) { perror("shmat"); return 1; }

	signals();
	printf("Потребитель System V PID=%ld\n", (long)getpid());

	while (!stop) {
		int got = 0, finished = 0;

		if (lock_sem(semid) == -1)
			break;

		for (int i = 0; i < sh->block_count; i++) {
			Block *b = &sh->blocks[i];
			if (b->count > 0) {
				int min = b->values[0], max = b->values[0], n = b->count;
				for (int j = 1; j < n; j++) {
					if (b->values[j] < min) min = b->values[j];
					if (b->values[j] > max) max = b->values[j];
				}
				b->count = 0;
				printf("PID=%ld: набор %d, min=%d, max=%d\n",
				       (long)getpid(), i + 1, min, max);
				fflush(stdout);
				got = 1;
				break;
			}
		}

		finished = sh->finished;
		unlock_sem(semid);

		if (finished) break;
		usleep(got ? 220000 : 120000);
	}

	shmdt(sh);
	printf("Потребитель %ld завершил работу\n", (long)getpid());
	return 0;
}

int main(int argc, char **argv) {
	if (argc != 2 ||
	    (strcmp(argv[1], "producer") && strcmp(argv[1], "consumer"))) {
		fprintf(stderr, "Использование: %s producer|consumer\n", argv[0]);
		return 1;
	}
	return strcmp(argv[1], "producer") == 0 ? producer() : consumer();
}