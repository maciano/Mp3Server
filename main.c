#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "tag.h"
#include "sem.h"
#include "functions.h"

#define App_Exception(val) do { err=val; goto error; } while (0)
#define port 8000

static struct ErrorMsg errors[] = {
	{ 255, "Error al crear socket" },
	{ 254, "Error al llamar a Bind." },
	{ 253, "Error al llamar a Listen." },
	{ 252, "Error en Accept." },
	{ 251, "Error al recibir la respuesta del client." },
	{ 250, "Error al enviar al CLiente." },
	{ 249, "Error creando memoria compartida." },
	{ 248, "Error desvinculando memoria compartida"},
	{ 247, "Error en la creacion de la memoria compartida"},
	{ 246, "Error eliminando la memoria compartida"},
	{ 245, "Error creando Key de memoria compartida." },
	{ 0, 0 }
};
struct gvari_m_t gv1m;

const char *ExceptionDetail(int code){
    int i;
    for (i=0; errors[i].errorNumb != 0; i++)
        if (errors[i].errorNumb == code) return errors[i].emsg;
    return NULL;
}

void handler(int signum,siginfo_t *si,void *data){

	liberar_shm();
	int *pids=(int*)data;
	for(pids;pids==NULL;pids++) kill(*pids,SIGTERM);
	while(wait(NULL) > 0);	
    	printf("Terminando ejecucion...!!\n");	
	exit(0); 
}


int main(int argc, char *const argv[]){
	gv1m.isPlaying=0;
	struct sockaddr_in sin;
	struct sockaddr_in pin;
	struct sigaction sint;
	sint.sa_flags=SA_RESTART | SA_SIGINFO;
	sint.sa_sigaction=handler;
	int i,len,nuhijos,err,val,sock_descriptor,contador,
	    temp_sock_descriptor,address_size;
	char buf[20000],bufatpl[15],bufforatpl[15],bufatplnum[15],fileName[100],c;
	char* ptr1;

	enlazar_proc_shm(0); //inicializo la variable de mem comp a 0

	/******** creo el set de semanaforos ******/
	crea_sem(crea_key(),1);
	/*********************************************/

	inicializar(L1p);
	inicializar(L2p);
	
	nuhijos=atoi(argv[1]);
	int pids[nuhijos];

	//Creamos el socket
	sock_descriptor = socket(AF_INET,SOCK_STREAM,0);
	
	//Comprobamos que este creado
	if(sock_descriptor == -1)
	{
		App_Exception(255);
	}
	
	bzero(&sin,sizeof(sin));
	
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	
	// Unimos el nuevo socket con el puerto 8000
	if(bind(sock_descriptor,(struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		App_Exception(254);
	}
	
	// Escuchamos en el nuevo socket en el puerto 8000, cola de hasta 20 msg
	if (listen(sock_descriptor,20)==-1)
	{
		App_Exception(253);
	}
	
	printf("Aceptando conexiones ...\n");

	/// LO CONVERTIMOS EN PRE-FORK 	
	for(i=0;i<nuhijos;i++) {
		switch((pids[i]=fork())) {
			case 0:
				// Bucle infinito que a la espera de conexiones de socket por parte de clientes
				while(1){
					bzero(buf,sizeof(buf));
					// Obtiene un socket temporal para administrar las peticiones del cliente
					temp_sock_descriptor=accept(sock_descriptor,(struct sockaddr *)&pin, &address_size);
		
					if (temp_sock_descriptor==-1){
						App_Exception(252);
					}
					// Recibe datos del cliente
					if(recv(temp_sock_descriptor,buf,20000,0)==-1){
						App_Exception(251);
					}
					printf("\n ** Recibido del cliente:%s\n **",buf);
					if(ContadorCaracteres(buf,'*') > 0){
						 strcpy(fileName,GetFileName(buf));
						 ConvertMp3Name(fileName,ContarMP3());
					}
					c = buf[0];
					switch(c){	 
						case 'l':
							strncpy(buf,lista_temasServer(),sizeof(buf));
							break;
						case 'p':
							strncpy(buf,mostrar_playlist(),sizeof(buf));
							break;
						case 'a':
				     			strcpy(bufforatpl,buf);
							ptr1=strtok(bufforatpl," ");
							strcpy(bufatpl,ptr1);
							if ((ptr1=strtok(NULL," "))==NULL) printf (" ERROR: NO HAY MAS TOKENS");
							strcpy(bufatplnum,ptr1);	
					  		agregar_playlist(atoi(bufatplnum));
							len=strlen(buf);
							send(temp_sock_descriptor,buf,len,0);
							printf("\nMAIN-entero:%d\n",valor_var_shm());
							if(valor_var_shm() == 0){
								enlazar_proc_shm(2);
								playmp3();
							
							}
						
			    		}

					len=strlen(buf);
					// Devuelve datos al cliente
					if(send(temp_sock_descriptor,buf,len,0)==-1){
						App_Exception(250);
					}
					// Cierra el socket temporal 
					close(temp_sock_descriptor);
					}
				}
			}
	sigaction(SIGINT,&sint,(void*)pids);		
	while(1) pause();
	return 0;
	//************* FIN RANGO DE PRE-FORK **********************

	error:
	 printf("%s\n",ExceptionDetail(err));
	 exit(1);
}

