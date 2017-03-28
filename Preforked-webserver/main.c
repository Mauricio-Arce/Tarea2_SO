#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>

int numeroProcesos = 0;
int puerto = 0;
char* path = "";
char* tipoProtocolo = "";
char respuesta[] = "Hola, funcione!!\n";

int verificarProtocolo();
void obtenerArgumentos(int cantidad, char* argumentos[]);
void iniciarServer();

int main(int argc, char* argv[])
{
    obtenerArgumentos(argc, argv);
    return 0;
}

int verificarProtocolo(){
    int protocoloValido = 0;

    char* protocolosMinus[6] = {"ftp", "ssh","smtp","dns","telnet","snmp"};
    char* protocolosMayus[6] = {"FTP", "SSH","SMTP","DNS","TELNET","SNMP"};

    int cantidadProtocolos = sizeof(protocolosMinus);
    for(int i = 0; i < 6; i++){
         if(strcmp(tipoProtocolo, protocolosMinus[i]) == 0 || strcmp(tipoProtocolo, protocolosMayus[i]) == 0){
            protocoloValido = 1;
            break;
        }
    }

    return protocoloValido;
}


void obtenerArgumentos(int cantidad, char* argumentos[]){
    char* argumentoAnterior = "";

    for(int i = 1; i < cantidad; i++){
        char* argumentoActual = argumentos[i];

        if(strcmp(argumentoAnterior,"-n") == 0 && strcmp(argumentoActual,"-w") != 0
           && strcmp(argumentoActual,"-p") != 0 && strcmp(argumentoActual,"-t") != 0){

            numeroProcesos = atoi(argumentoActual);

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

    if(numeroProcesos == 0 || puerto == 0 || path == "" || tipoProtocolo == ""){
        printf("El número de hilos, el puerto, el directorio root o el tipo de protocolo no se ingreso correctamente!!\n");
    }

    else if (verificarProtocolo() == 0){
        printf("El protocolo ingresado no es valido!! \n");
    }

    else{
        iniciarServer();
    }

}

void iniciarServer(){
    int one = 1, client_fd;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);

    //Se crea el socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0){
        printf("No se creo el socket");
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
    listen(sock, 5);

    while (1) {
        //Se acepta la conexión
        client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
        printf("Hay conexiones\n");

        if (client_fd == -1) {
            printf("No se aceptó la conexión");
            continue;
        }

        //Retorna un mensaje al cliente
        write(client_fd, respuesta, sizeof(respuesta) - 1);
        close(client_fd);
    }
}
