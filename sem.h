#include <sys/types.h>	/* para key_t */

void sem_P (int);
void sem_V (int);
int crea_sem ( key_t,int);
int abre_sem (key_t);
int crea_key(void);
void eliminar_semaforo();
