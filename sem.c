#include <sys/ipc.h>
#include <sys/sem.h>
#define PERMISOS 0666

/* crea_sem: abre o crea un semáforo */
int crea_key(void){
	key_t key;

	key=ftok(".", 0); //crea el key de memoria compartida
	if (key<0){ 
		perror("\n key no generada");
		return -1;
	} else {
		//printf("\nKey obtenida=%d\n",key);
		return key;
	}
}

int sema_valor(int semid){	
	/*int valor;

 	valor=semctl (semid, 0, , valor_inicial); //hacer consulta
	return valor;*/
}

int crea_sem( key_t clave, int valor_inicial ){
	/* Abre o crea un semáforo... */
	 int semid = semget(clave,			/* con una cierta clave */
				1,			/* con un solo elemento */
				IPC_CREAT|PERMISOS	/* lo crea (IPC_CREAT) con   unos PERMISOS */
				); 

	 if (semid==-1){	
		 return -1;
	 }
	 /* Da el valor inicial al semáforo */
	 semctl (semid, 0, SETVAL, valor_inicial);
	 return semid; 			 							
}

/* abre_sem: Abrir un semáforo que otro proceso ya creó */
int abre_sem(key_t clave)	
{
	 return semget(clave,1,0);
}


/* Operaciones P y V */
void sem_P(int semid){	/* Operación P */

	struct sembuf op_P[] = {
	  0, -1, 0	/* Decrementa semval o bloquea si cero [Num. sem, (-1) Dec/(1) Incre, Opciones] */
	};

	semop (semid,op_P,1); //[id de conj. de sem,tabla de operaciones,numero de oper]
}

void sem_V(int semid)	/* Operación V */
{
	struct sembuf op_V[] = {
	   0, 1, 0		/* Incrementa en 1 el semáforo */
	};
	semop (semid,op_V,1);
}

void eliminar_semaforo() {
	 int semid = semget(crea_key(),1,IPC_CREAT|PERMISOS); 
	 printf("\n SEMID:%d",semid);
	 semctl (semid, 0, IPC_RMID, 0); /* Elimino el set de semanaforos */
}
