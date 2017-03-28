#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum {FALSE, TRUE} Bool;

/* 
 * funcion que llama a curl con el url pasado como argurgumento.
 */
int curl_llamada( char *url){
	printf("URL utilizado: %s\n", url);
	char comando[400];			//tama;o del comando
	strcpy(comando, "curl ");	
	strcat(comando, url );
	strcat(comando, " > resultado.html" ); // al final quedaria: curl [url] > resultado.html
	printf("%s\n", comando); 
	system(comando);
	return 0;
}



int main( int argc, char * argv [] ) {
	printf( "argc = %d\n", argc );
	
	if (argc == 3){
		if (!(strcmp(argv[1],"-u") == 0)){
			printf ("Parametro -u extraviado\n");
			return 0;
		} 
		curl_llamada (argv[2]);  //llamada a la funcion principal
		} else {
			printf ("Muchos/Pocos parametros\n");
			printf ("Sintaxis correcta es: ./httpclient -u <url>\n");

		}
	return 0;
}