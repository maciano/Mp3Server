void ConvertMp3Name(char*,int);
char* lista_temasServer(void);
int selecc_arch(struct dirent*);
char* mostrar_playlist(void);
void agregar_playlist(int);
char* Sacar_tags(char*);
int get_id(char*);
char* get_name(int);
int ContarMP3(void);
void playmp3();
char* GetFileName(char*);
void gvfInit(void);
void inicializar(char*);
int ContadorCaracteres(char*,char);
void enlazar_proc_shm(int);
int valor_var_shm(void);
void liberar_shm(void);

#define L1 	"playlist1.txt"
#define L2 	"playlist2.txt"
#define L1p 	"./mp3/playlist1.txt"
#define L2p 	"./mp3/playlist2.txt"
#define mpg123  "/usr/bin/mpg123"
#define Relpath "./mp3/"

typedef struct gvari_f_t {
	char PATH[100];
	char str[20000];
	char str2[20000];
	int listaflag;
	int playfile;
};

typedef struct gvari_m_t{
	int isPlaying;
};
typedef struct ErrorMsg{
	int errorNumb;
	const char *emsg;
};
