#!/usr/bin/env python

import threading
from subprocess import call
import argparse

## funcion que ejecutaran los hilos
def worker( *comando):
	call(comando) ### comando = ["./httpclient", "-u", <url>, "-o" <output_name>"]
	return

## manejo de argumentos vio argparse library
def manager_de_argumentos():
	parser = argparse.ArgumentParser(description='Stress de Httpclient')
	parser.add_argument('-n', action='store', dest='cantidad_hilos', help='Cantidad de hilos que se utilizaran', type=int, required=True)
	parser.add_argument('comando', metavar='Comando_a_usar', type=str, nargs=2, help='Commando a ejecutar y sus parametros')
	lista_comandos = parser.parse_args()
	return lista_comandos 

## funcion generadora de hilos, cada hilo crea un archivo con el id del hilo
def generar_hilos( cantidad_hilos, comando ):
	salida = comando[4]
	threads = []
	for i in range( cantidad_hilos ):
		comando[4] = str(i)+salida
		t = threading.Thread( target=worker , args = (comando) )
		threads.append(t)
		t.start()

def main():
	lista_comandos = manager_de_argumentos()
	generar_hilos (lista_comandos.cantidad_hilos , [lista_comandos.comando[0]] + lista_comandos.comando[-1].split() )
	
if __name__ == "__main__":
	main()