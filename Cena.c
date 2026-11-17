
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#define N 5
sem_t *tenedores[N];
sem_t *sala;
void *filosofo(void *arg)
{
    int num = *(int *)arg;
    for (int i = 0; i < 3; i++)
    {
        printf("Filosofo %d esta pensando\n", num);
        sleep(1);
        sem_wait(sala);
        sem_wait(tenedores[num]);
        sem_wait(tenedores[(num + 1) % N]);
        printf("Filosofo %d esta comiendo\n", num);
        sleep(2);
        sem_post(tenedores[num]);
        sem_post(tenedores[(num + 1) % N]);
        sem_post(sala);
        printf("Filosofo %d termino de comer\n", num);
    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t filosofos[N];
    int numeros[N];
    sala = sem_open("/sala", O_CREAT, 0644, 4);
    if (sala == SEM_FAILED)
    {
        perror("Error al crear semaforo sala");
        return 1;
    }
    for (int i = 0; i < N; i++)
    {
        char nombre[30];
        snprintf(nombre, sizeof(nombre), "/tenedor%d", i);
        tenedores[i] = sem_open(nombre, O_CREAT, 0644, 1);
        if (tenedores[i] == SEM_FAILED)
        {
            perror("Error al crear tenedor");
            return 1;
        }
    }
    for (int i = 0; i < N; i++)
    {
        numeros[i] = i;

        pthread_create(&filosofos[i], NULL, filosofo, &numeros[i]);
    }
    for (int i = 0; i < N; i++)
    {
        pthread_join(filosofos[i], NULL);
    }
    for (int i = 0; i < N; i++)
    {
        char nombre[30];
        snprintf(nombre, sizeof(nombre), "/tenedor%d", i);
        sem_close(tenedores[i]);
        sem_unlink(nombre);
    }
    sem_close(sala);
    sem_unlink("/sala");
    return 0;
}
