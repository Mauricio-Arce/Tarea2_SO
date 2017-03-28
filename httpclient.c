#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum {FALSE, TRUE} Bool;

/* 
 * funcion que llama a curl con el url pasado como argurgumento.
 */
int curl_llamada( char *url, char *output){
	printf("URL utilizado: %s\n", url);
	char comando[400];			//tama;o del comando
	strcpy(comando, "curl ");	
	strcat(comando, url );
	strcat(comando, " > resultados/" ); 
	strcat(comando, output); // al final quedaria: curl [url] > <output_name>.html
	strcat(comando, ".html");
	printf("%s\n", comando); 
	system(comando);
	return 0;
}



int main( int argc, char * argv [] ) {
	if (argc == 5){
		if (!(strcmp(argv[1],"-u") == 0)){
			printf ("Parametro -u extraviado\n");
			return 0;
		}
		if (!(strcmp(argv[3],"-o") == 0)){
			printf ("Parametro -o extraviado\n");
			return 0;
		}
		curl_llamada (argv[2], argv[4]);  //llamada a la funcion principal
	} else {
		printf ("Muchos/Pocos parametros\n");
		printf ("Sintaxis correcta es: ./httpclient -u <url>\n");
	}
	return 0;
}