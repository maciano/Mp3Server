#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include "tag.h"
#include "functions.h"
#include "sem.h"

struct gvari_f_t gv1={"./mp3/","","",1,2};

void playmp3(){
	int isList1Removed,isList2Removed,pidh1,pidh2 = 0,
		playList1Exist =0,playList2Exist =0;	
	
	if((pidh1 = fork())==-1){
		perror("fallo del fork1\n");
		exit(EXIT_FAILURE);
	} else if(pidh1 == 0){
		enlazar_proc_shm(1);
		remove(L2p);
		inicializar(L2p);
		execl(mpg123,mpg123,"-q","-@",L1p,NULL);
  	} else {
		waitpid(pidh1,NULL,0);
		printf("\npraymp3-3\n");
		if((pidh2 = fork())==-1){
			perror("fallo del fork2\n");
			exit(EXIT_FAILURE);
		} else if (pidh2 == 0){
			enlazar_proc_shm(2);
			remove(L1p);
			inicializar(L1p);
			execl(mpg123,mpg123,"-q","-@",L2p,NULL);
	    	} else {   
			waitpid(pidh2, NULL,0);
			remove(L2p);
			inicializar(L2p);
			if(ListaVacia(L1p)){
				enlazar_proc_shm(0);
			} else {
			  	playmp3();
			}	
		} 
	} 
}
int valor_var_shm()
{
	int *entero,r,shmid,val;

	shmid = shmget(crea_key(), sizeof(int),  IPC_CREAT|0666);
	if (shmid == -1)
	{
		perror("\n Error memoria compartida");
	} else {
		//printf("\shmid:%d\n",shmid);
	}

	entero = (int *)shmat(shmid, NULL, 0);
	if (entero == (int *)-1) {
    		perror("Funtion:atpl-Obteniendo dirección de memoria compartida");
    		return -1;
	}
	val = (*entero);
	r = shmdt(entero);
	if (r == -1){
   		perror("Error desvinculando memoria compartida");
	}
	return val;
}

void liberar_shm()
{	
	/** Elimino set de semaforos **/
	eliminar_semaforo();
	/** Elimino memoria compartida **/
	int r,shmid;
	shmid = shmget(crea_key(), sizeof(int),0666);
	r = shmctl(shmid, IPC_RMID, NULL);
	if (r == -1) perror("Borrando ERROR!!");
}

void enlazar_proc_shm(int value){
		int *entero,r1,r2;
		int shmid;
		int r;
	
		shmid = shmget(crea_key(), sizeof(int),  IPC_CREAT|0666);
		if (shmid == -1){
			//App_Exception(249);
			perror("\n Error memoria compartida");
		} else {
			printf("\shmid:%d\n",shmid);
		}

		entero = (int *)shmat(shmid, NULL, 0);
		if (entero == (int *)-1){
    			perror("Error obteniendo dirección de memoria compartida");
    			return -1;
		}
		(*entero) = value;
		r1 = shmdt(entero);
		if (r1 == -1) {
    			perror("Error desvinculando memoria compartida");
		}
}

char* get_name(int track)
{
	char trackname[120];
	int contar,i,numtema,numarch;
	struct dirent **archivos=NULL;

	bzero(gv1.str,sizeof(gv1.str));     
	bzero(trackname,sizeof(trackname));     
	contar = scandir(Relpath, &archivos,selecc_arch,NULL);

	for (i=1; i < contar + 1; ++i)
	{
		numarch=atoi(archivos[i-1]->d_name);
		if (numarch==track)
		{ 
			strncpy(trackname,archivos[i-1]->d_name,sizeof(trackname));
			return trackname;
		}
	}
	strncpy(trackname,"NO EXISTE",9);
	return trackname;
}


int get_id (char *filename)
{
	int id;
	id=atoi(filename);
	return id;
}


char* Sacar_tags(char* filename)
{
	char data[20000], dir[200];
	struct mp3tag *mp3info;
	int numid;

    strncpy(dir,gv1.PATH,sizeof(dir));  
    strcat(dir,filename);
    mp3info = get_mp3tag(dir);
    numid=get_id(filename);
    
    if((mp3info->artista != NULL) && (mp3info->tema != NULL)){
	    printf("\n%.4d - %s - %s",numid,mp3info->artista,mp3info->tema);	
	    snprintf(data,sizeof(data),"\n%.4d - %s - %s",numid,mp3info->artista,mp3info->tema);	
    }
    return data;
}


char* mostrar_playlist()
{
	FILE *file;
	char c[50], temp2[20000];
	int pflag,i;//=gv1.listaflag,i;
	bzero(gv1.str2,sizeof(gv1.str2));
        bzero(c,sizeof(c));
	pflag = valor_var_shm();
	
	for (i=0;i<2;i++){	
		// vemos que lista esta sonando
		if (pflag == 1){
			file = fopen(L1p, "r"); 
		} else {
			file = fopen(L2p, "r"); 
		}
		while(fgets(c,50, file)!=NULL){ 
		    printf("*** Archivo leido:%s", c);        
		    strtok(c,"\n");	
        	    strncpy(temp2,Sacar_tags(c),sizeof(temp2));	
        	    strcat(gv1.str2,temp2);
		    bzero(c,sizeof(c));
		}
		fclose(file);
		if(pflag == 1){
			pflag=2;
		} else {
			pflag=1;
		}
	}
	return gv1.str2;
}
char* leer_archivo(char *path){
	FILE *fd;
	fd = open(path,O_RDWR|O_CREAT, 0666);
	char buf[20000];
	int c;
	printf("Hola");
	while ((c = fgetc(fd))!=EOF)
	{
		printf("SOY C:%d\n",c);
		strcat(buf,c);
	}
	close(fd);
	return buf;
}

void escribir_archivo(char *path, char *buf){
	FILE *fd;
	fd = open(path,O_RDWR,0666);
	write(fd,buf,strlen(buf));
	close(fd);
}

void agregar_playlist(int track){
	char nombre[120],buf[20000];
	memset(nombre,0,sizeof(nombre));
	strncpy(nombre,get_name(track),120);
	if (strncmp(nombre,"NO EXISTE",9)==0){
		perror("fallo al agregar a la lista\n");
		return;
	}
	printf("\n Nombre: %s\n",nombre);
	
	//********************* RECURSO CRITICO ******************************/
	int sem_id;	
	sem_id=abre_sem(crea_key());
	sem_P(sem_id);
        printf("hola estoy por llerr\n");
	strncat(nombre,"\n",2);
	printf("\n Nombre2: %s\n",nombre);
	
	if (valor_var_shm()==1){
		strcpy(buf,leer_archivo(L2p));//,sizeof(buf));
		strcat(buf,nombre);//,sizeof(nombre));
		escribir_archivo(L2p,buf);
	} else if(valor_var_shm()==2  || valor_var_shm()==0){
		strcpy(buf,leer_archivo(L1p));//,sizeof(buf));
		strcat(buf,nombre);//sizeof(nombre));
		escribir_archivo(L1p,buf);	
	} else {
		return 0;
	}
	sem_V(sem_id);
}

void ConvertMp3Name (char *filename,int totalcount){
	char newname[120],patho[120],pathd[120];

	strncpy(patho,gv1.PATH,120); 
	strncpy(pathd,gv1.PATH,120); 
	snprintf(newname, 120, "%.4d", (totalcount+1));
	strncat(newname,"-",1);
	strncat(newname,filename,sizeof(newname));
	strncat(patho,filename,sizeof(patho));
	strncat(pathd,newname,sizeof(pathd)); 
	printf(" NUEVO NOMBRE: %s\n", pathd);
	rename(patho,pathd);
}

int ContarMP3()
{ 
	int contar;
	struct dirent **archivos2=NULL;

	contar = scandir(Relpath, &archivos2,selecc_arch,NULL);
	return contar;
}


char* lista_temasServer(void)
{
	int contar,i,numtema;
	struct dirent **archivos=NULL;
	struct mp3tag *mp3;
	char directorio[200],temp[20000];
	
	bzero(gv1.str,sizeof(gv1.str));        
	contar = scandir(Relpath, &archivos,selecc_arch,NULL);
        printf(" Cant. de archivos mp3 en el Server: %d\n\n",contar);

	for (i=1;i<contar+1;++i){
		strncpy(directorio,gv1.PATH,200);  
		strncat(directorio,archivos[i-1]->d_name,200);
		mp3 = get_mp3tag(directorio);
		numtema=get_id(archivos[i-1]->d_name);	// agregado para ordenar bien
		if((mp3->artista == NULL) || (mp3->tema == NULL)){
			 continue;
		}
		snprintf(temp,20000,"\n%.4d - %s - %s",numtema,mp3->artista,mp3->tema);
		strncat(gv1.str,temp,sizeof(gv1.str));
	}
	return gv1.str;
}

int selecc_arch(struct dirent *entry){
	char *ptr;
	if ((strncmp(entry->d_name, ".",1)== 0) ||(strncmp(entry->d_name, "..",2) == 0)){
		return 0;
	}
	// Revisar las extensiones de los archivos
	ptr = rindex(entry->d_name, '.'); // Probar que tenga un punto //
	if ( (ptr != NULL ) &&  ((strncmp(ptr, ".mp3",4) == 0)) ){
                return 1;
        } else {
                return 0;
        }
}

char* GetFileName(char *Path)
{
	char s2[2] = "/",fileName[200],*ptr;;
	int count;
	
	ptr = strtok( Path, s2 );    // Primera llamada => Primer token
	while((ptr = strtok( NULL, s2 )) != NULL ){
		strcpy(fileName,ptr);	
	}
	ptr = strtok(fileName,"****");
	return ptr;
}

void CreatePlayListFiles(char *filePath){
	creat(filePath, 0666);
}

void inicializar(char *listName){
	CreatePlayListFiles(listName);
}

int ContadorCaracteres(char *cadena, char caracterAContar){
	int i,count=0;

	for(i=0;cadena[i]!=0;i++){	
		if(cadena[i] == caracterAContar){
			count++;
		}
	}
	return count;
}

int ListaVacia(char *lpath){
	FILE *fo;
	int nbyte;
	char buffer[5];
	
	if((fo=open(lpath,O_RDONLY))==-1){
		perror("No se pudo abrir");
		return 0;
	} else if((nbyte=read(fo,buffer,5))>0){
		return 0;
	} else {
		return 1;				
	}
}
