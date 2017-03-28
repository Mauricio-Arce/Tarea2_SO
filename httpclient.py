import os
import sys

#Comandos de entrada
#Comando GET: python httpcliente.py -u pathHttp
#Comando DELETE: python httpcliente.py -u DELETE/delete pathHttp
#Comando PUT: python httpcliente.py -u PUT/put -d argumento=valor pathHttp
#Comando POST: python httpcliente.py -u --data @archivoAsubir ptahHttp

argumentos = sys.argv
argumentos.pop(0)
argumentos.pop(0)

cantidadArgumentos = len(argumentos)
mensajeCurl = "curl --silent "

if(argumentos[0] == "DELETE" or argumentos[0] == "delete"
   or argumentos[0] == "PUT" or argumentos[0] == "put"):
    mensajeCurl += "-X"

for i in range(cantidadArgumentos):
    mensajeCurl += argumentos[i] + " "

resultadoHttp = os.popen(mensajeCurl).read()

print(resultadoHttp)



