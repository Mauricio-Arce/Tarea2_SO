#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>

int conexionesMax = 1000;
int procesosMaximos = 0;
int* listaProcesos;
int puerto = 0;
char* path = "";
char* tipoProtocolo = "";


typedef struct ComandoCliente{
    char metodo[10];
    char url[60];
} CmdCliente;

void* imprimirHilos(void* hilosID);
void obtenerArgumentos(int cantidad, char* argumentos[]);
void iniciarServer();
void verificarMetodoHTTP(int cliente);
void headerSalida(int cliente, char urlArchivo[]);
void metodoDelete(int cliente, char urlArchivo[]);
void metodoGet(int cliente, char urlArchivo[]);
void metodoPut(int cliente, char urlArchivo[]);
void metodoPost(int cliente, char urlArchivo[]);
void iniciarSocket();
void iniciarSWForked();
int existeArchivo(char urlArchivo[]);
int verificarProtocolo();
int verificarExtension(char urlArchivo[]);
char* tipoExtension(char* extension);
char* extension(char extensionEntrada[]);

CmdCliente extraerEntradaCliente(char entrada[]);

int main(int argc, char* argv[]){
    obtenerArgumentos(argc, argv);
    return 0;
}

void *imprimirHilos(void *hiloID){
    long id = (long) hiloID;
    printf("Hilo #%ld!\n", id);
}

int verificarExtension(char urlArchivo[]){
    int tieneExtension = 0;
    int largoCadena = strlen(urlArchivo);
    int i;

    char caracter;

    for(i = largoCadena - 6; i < largoCadena; i++){
        caracter = urlArchivo[i];
        if(caracter == '.'){
            tieneExtension = 1;
            break;
        }
    }

    return tieneExtension;
}

int existeArchivo(char urlArchivo[]){
    FILE* file;
    int tieneExtension = verificarExtension(urlArchivo);
    int existe = 0;

    file = fopen(urlArchivo,"r");

    if(tieneExtension == 1 && file != NULL){
        fclose(file);
        existe = 1;
    }

    return existe;
}


void metodoDelete(int cliente, char urlArchivo[]){
    FILE *file;
    static char rutaArchivo[1];
    char msnArchivoNoExiste[] = "\n<html><body><h1>El archivo ingresado no existe!!</h1></body></html>";
    char msnArchivoEliminador[] = "\n<html><body><h1>El archivo a sido eliminado!!</h1></body></html>";
    sprintf(rutaArchivo,"%s%s",path, urlArchivo);

    int fexist = existeArchivo(rutaArchivo);

    if(fexist == 0){
        write(cliente, msnArchivoNoExiste, strlen(msnArchivoNoExiste));
    }

    else{
        remove(rutaArchivo);

        headerSalida(cliente, rutaArchivo);
        write(cliente, msnArchivoEliminador, strlen(msnArchivoEliminador));
    }
}

void metodoGet(int cliente, char urlArchivo[]){
    FILE* file;
    static char rutaArchivo[1];

    sprintf(rutaArchivo,"%s%s",path, urlArchivo);

    int fexist = existeArchivo(rutaArchivo);


    char rutaCopia[1];
    strcpy(rutaCopia, rutaArchivo);

    char* ext = extension(rutaCopia);
    char* tipoExt = tipoExtension(ext);

    if(fexist && (strcmp(tipoExt,"text/") == 0) ){
        sprintf(rutaArchivo,"%s.%s",rutaCopia, ext);

        file = fopen(rutaArchivo,"r");

        char contenidoArchivo[150];
        char linea[100];

        while(fgets(linea, 100, file)){
            strcat(contenidoArchivo, linea);
        }

        fclose(file);

        headerSalida(cliente, rutaArchivo);

        write(cliente, contenidoArchivo, strlen(contenidoArchivo));
    }
    else if(strcmp(tipoExt,"text/")){
        char msnArchivoExtensionInvalida[] = "<html><body><h1>El archivo ingresado no puede ser mostrado!!</h1></body></html>";
        write(cliente, msnArchivoExtensionInvalida, strlen(msnArchivoExtensionInvalida));
    }
    else{
        char msnArchivoNoExiste[] = "<html><body><h1>El archivo ingresado no existe!!</h1></body></html>";
        write(cliente, msnArchivoNoExiste, strlen(msnArchivoNoExiste));
    }
}

void metodoPost(int cliente, char urlArchivo[]){
    headerSalida(cliente, urlArchivo);
}

void metodoPut(int cliente, char urlArchivo[]){
    headerSalida(cliente, urlArchivo);
}


int verificarProtocolo(){
    int protocoloValido = 0;

    char* protocolosMinus[] = {"ftp", "ssh","smtp","dns","telnet","snmp"};
    char* protocolosMayus[] = {"FTP", "SSH","SMTP","DNS","TELNET","SNMP"};

    int cantidadProtocolos = sizeof(protocolosMinus);
    int i;
    for(i = 0; i < 6; i++){
         if(strcmp(tipoProtocolo, protocolosMinus[i]) == 0 || strcmp(tipoProtocolo, protocolosMayus[i]) == 0){
            protocoloValido = 1;
            break;
        }
    }

    return protocoloValido;
}

void obtenerArgumentos(int cantidad, char* argumentos[]){
    char* argumentoAnterior = "";

    int i;
    for(i = 1; i < cantidad; i++){
        char* argumentoActual = argumentos[i];

        if(strcmp(argumentoAnterior,"-n") == 0 && strcmp(argumentoActual,"-w") != 0
           && strcmp(argumentoActual,"-p") != 0 && strcmp(argumentoActual,"-t") != 0){

            procesosMaximos = atoi(argumentoActual);

        }
        else if(strcmp(argumentoAnterior,"-w") == 0 && strcmp(argumentoActual,"-n") != 0
                && strcmp(argumentoActual,"-p") != 0 && strcmp(argumentoActual,"-t") != 0){

            path = argumentoActual;

        }
        else if(strcmp(argumentoAnterior,"-p") == 0 && strcmp(argumentoActual,"-n") != 0
                && strcmp(argumentoActual,"-w") != 0 && strcmp(argumentoActual,"-t") != 0){

            puerto = atoi(argumentoActual);

        }
        else if(strcmp(argumentoAnterior,"-t") == 0 && strcmp(argumentoActual,"-n") != 0
                && strcmp(argumentoActual,"-w") != 0 && strcmp(argumentoActual,"-t") != 0){

            tipoProtocolo = argumentoActual;

        }

        argumentoAnterior = argumentoActual;
    }

    if(procesosMaximos == 0 || puerto == 0 || path == "" || tipoProtocolo == ""){
        printf("El número de hilos, el puerto, el directorio root o el tipo de protocolo no se ingreso correctamente!!\n");
    }

    else if (verificarProtocolo() == 0){
        printf("El protocolo ingresado no es valido!! \n");
    }

    else{
        iniciarSocket();
    }

}


char* tipoExtension(char* extension){
    static char extensionSalida[1];
    char* extensionesTexto[8] = {"js","xml","css","asp","cgi","html","htm","txt"};

    int i;
    int banderaCambio = 0;

    for(i = 0; i < 8; i++){
        char* extensionActual = extensionesTexto[i];
        if(strcmp(extension, extensionActual) == 0){
            sprintf(extensionSalida,"text/");
            banderaCambio = 1;
            break;
        }
    }

    if(banderaCambio == 0){
        sprintf(extensionSalida,"image/");
    }

    return extensionSalida;
}

char* extension(char urlArchivo[]){
    char* splitURL = strtok(urlArchivo,".");
    char* ext;
    int i = 0;

    while(i < 2){
        if(i == 1){
            ext = malloc(strlen(splitURL)+1);
            strcpy(ext, splitURL);
            break;
        }
        splitURL = strtok(NULL, ".");
        i += 1;
    }

    return ext;
}


void headerSalida(int cliente, char urlArchivo[]){
    FILE* file;
    static char header[1];

    char* ext = NULL;
    char* tipoExt = NULL;


    struct stat st;
    stat(urlArchivo, &st);

    int largo = st.st_size;


    ext = extension(urlArchivo);
    tipoExt = tipoExtension(ext);

    if(strcmp(tipoExt, "text/") == 0){
        sprintf(header, "200 OK HTTP/1.1\nContent-Type: text/%s \nContent-Length: %d\n", ext, largo);
    }
    else{
        sprintf(header, "200 OK HTTP/1.1\nContent-Type: image/%s \nContent-Length: %d\n", ext, largo);
    }

    write(cliente, header, strlen(header));
}

CmdCliente extraerEntradaCliente(char entrada[]){
    CmdCliente parametros;
    int bandera = 0;
    int i = 0;

    char** salida;
    char* splitEntrada = strtok(entrada," ");
    char* metodo;
    char* url;

    while(i < 2){
        if(i == 0){
            metodo = malloc(strlen(splitEntrada)+1);
            strcpy(metodo, splitEntrada);
        }
        else if(i == 1){
            url = malloc(strlen(splitEntrada)+1);
            strcpy(url, splitEntrada);
            break;
        }
        splitEntrada = strtok(NULL, " ");
        i += 1;
    }


    strcpy(parametros.metodo, metodo);
    strcpy(parametros.url, url);

    return parametros;
}

void verificarMetodoHTTP(int cliente){
    static char entradaCliente[500];
    long respuestaCliente = read(cliente, entradaCliente, 500);

    CmdCliente parametros = extraerEntradaCliente(entradaCliente);

    if(strcmp(parametros.metodo,"GET") == 0 || strcmp(parametros.metodo,"get") == 0){
        metodoGet(cliente, parametros.url);
    }
    else if(strcmp(parametros.metodo,"PUT") == 0 || strcmp(parametros.metodo,"put") == 0){
        printf("Put\n");
        metodoPut(cliente, parametros.url);
    }
    else if(strcmp(parametros.metodo,"POST") == 0 || strcmp(parametros.metodo,"post") == 0){
        printf("Post\n");
        metodoPost(cliente, parametros.url);
    }
    else if(strcmp(parametros.metodo,"DELETE") == 0 || strcmp(parametros.metodo,"delete") == 0){
        metodoDelete(cliente, parametros.url);
    }
    else{
        char mensajeFallo[] = "El metodo digitado no esta contemplado!!\n";
        write(cliente, mensajeFallo, strlen(mensajeFallo));
    }
}

void iniciarSWForked(){
    int procesoActual = 0;
    pid_t procesoId;

    listaProcesos = (int*)malloc(procesosMaximos*sizeof(int));

    while(procesoActual < procesosMaximos){
        procesoId = fork();

        if(procesoId < 0){
            printf("Problema al crear los fork()\n");
        }
        else{
            listaProcesos[procesoActual] = procesoId;
            procesoActual +=1;
        }
    }
}

void iniciarSocket(){
    int one = 1, cliente;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);

    //Se crea el socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0){
        printf("No se creo el socket\n");
    }
    else{
        printf("Server Iniciado!!\n");
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,&one, sizeof(int));

    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(puerto);

    //Se asigna el socket a una dirección
    if(bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1){
        close(sock);
        printf("No se pudo crear la conexión!!\n");
    }

    //Socket escucha las conexiones ingresadas
    listen(sock, conexionesMax);

    iniciarSWForked();

    while (1) {
        //Se acepta la conexión
        cliente = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);

        if (cliente < 0) {
            printf("No se aceptó la conexión\n");
            continue;
        }
        else{
            int nuevoProceso = -1;

            for(int i = 0; i < sizeof(listaProcesos); i++){
                if(listaProcesos[i] == 0){
                    listaProcesos[i] = 1;
                    nuevoProceso = i;
                    break;
                }
            }

            if(nuevoProceso == -1){
                char* msnMaximosAtendidos = "Lo sentimos, se ha sobrepasado la cantidad máxima de procesos y no se puede atender su solicitud!!\n";
                write(cliente, msnMaximosAtendidos, strlen(msnMaximosAtendidos));
                close(cliente);
            }
            else{
                printf("Atendiendo al cliente\n");
                verificarMetodoHTTP(cliente);
                close(cliente);
                printf("Conexión cerrada\n");
                listaProcesos[nuevoProceso] = 0;
            }
        }
    }
}
