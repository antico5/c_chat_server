//Server

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include "module.h"
#include <time.h>

#define MAXBUFF 512
#define N_DE_CLIENTES 50
#define INTERVALO_CHEQUEO 5
#define CONECTADO 1
#define DESCONECTADO 2

sockaddr_in estructura_s;
SOCKET socket_s;
time_t tiempo = time(NULL);


struct _client
{
	bool		connected;
	sockaddr_in	address;
	SOCKET		socket;
	fd_set		socket_data;
	int         addrlen;
	int			x;
	char        ip[32];
	char        nick[32];     
};

_client* clientes[N_DE_CLIENTES];


bool Iniciar();
bool Aceptar();
void Enviar(_client* cliente , char* buffer);
void EnviarAConectados(char* buffer);
void Mensaje(char* buffer);
void Recibir(char* buffer);
void Procesar(_client* cliente , char* buffer);
void Check();
void Desconectar(_client* cliente);
void Terminar();
void ActualizarLista(char* usuario, int opcion);
void EnviarLista(_client* cliente);

bool Iniciar(){;
     for (int i=0;i<N_DE_CLIENTES;i++){
	     clientes[i] = new _client;
	     memset(clientes[i],0,sizeof(_client));
     }
     estructura_s.sin_family = AF_INET;
     estructura_s.sin_port = htons(1339);
     estructura_s.sin_addr.s_addr=INADDR_ANY;
     socket_s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
     
     if(socket_s==INVALID_SOCKET) {
          printf("[x]No se pudo crear el socket\n");
          return false;
     }
     printf("[O]Socket inicializado\n");
     
     if(bind(socket_s,(sockaddr*)&estructura_s,sizeof(sockaddr)) == SOCKET_ERROR) {
          printf("[x]Error bindeando\n");
          return false;
     }
     printf("[O]Socket bindeado\n");
     
     if (listen(socket_s,SOMAXCONN) == SOCKET_ERROR){
          printf("[x]Error poniendo a la escucha\n");
          return false;
     }
     printf("[O]Socket a la escucha\n");
     
     unsigned long b = 1;
     if (ioctlsocket(socket_s,FIONBIO,&b) == SOCKET_ERROR){
          printf ("[x]Error activando non-block\n");
          return false;
     }
     
     printf ("[O]Socket inicializado por Completo\n\n\n");
     return true;
}     

bool Aceptar(){   
	for(int i = 0;i<N_DE_CLIENTES;i++){
		if (clientes[i] -> connected != true){
			clientes[i]->addrlen = sizeof(sockaddr);
			clientes[i]->socket = accept(socket_s , (sockaddr*)&clientes[i]->address , &clientes[i]->addrlen);
			if (clientes[i]->socket != 0 && clientes[i]->socket != INVALID_SOCKET){
				sprintf(clientes[i]->ip, "%s", inet_ntoa(clientes[i]->address.sin_addr));
				FD_ZERO(&clientes[i]->socket_data);
				FD_SET(clientes[i]->socket,&clientes[i]->socket_data);
				clientes[i] -> connected = true;
				return true;
			} 
			return false;
		}
	}
    return false;
}

void Recibir(char* buffer){
	for (short i = 0; i<N_DE_CLIENTES; i++){
		if (clientes[i]->connected == true){
			if (FD_ISSET(clientes[i]->socket,&clientes[i]->socket_data)) {                  
				clientes[i]->x = recv(clientes[i]->socket,buffer,MAXBUFF-1,0);
				if (clientes[i]->x >0){   
					Procesar(clientes[i],buffer);
					memset(buffer,0,MAXBUFF);
				}
				else if (clientes[i]->x == 0){
					Desconectar(clientes[i]);
				}    
			} 
		}
	}
}

void Procesar(_client* cliente,char* buffer){
     SetColor(white); 
     if (!strcmp(buffer,""))
         return;
         
     if (!strcmp(buffer, "quit")){
          Desconectar(cliente);
          return;
     }         
     if (!strncmp(buffer,"-m",2)) {
         if (strlen(cliente->nick) > 0){
             char mensaje_f[MAXBUFF+64] = {0};          
             char* mensaje = buffer + 2;
             sprintf(mensaje_f,"%s (%s): %s" , cliente->nick, cliente->ip, mensaje);
             SetColor(white);
             Mensaje(mensaje_f); 
             memset(buffer,0,MAXBUFF);
         }
     }
     if (!strncmp(buffer,"-n",2)) {
         if (strlen(buffer) > 2){
             char* nuevonick = buffer+2;
             int orden;
             char mensaje_nuevo[MAXBUFF] = {0};
             memset(mensaje_nuevo,0,MAXBUFF);                       
             if (!strlen(cliente->nick)){
                 orden = 1;                                     
                 sprintf(mensaje_nuevo,"Cliente conectado: %s (%s).",nuevonick,cliente->ip);
             }                           
             else{
                 sprintf(mensaje_nuevo,"El cliente (%s) ha cambiado su nick de %s a %s.",cliente->ip,cliente->nick,nuevonick);
                 orden = 2;
             }
             SetColor(blue);
             Mensaje(mensaje_nuevo);
             Sleep(1);
             if(orden == 1){
                 EnviarLista(cliente);     
                 ActualizarLista(nuevonick,CONECTADO);
             }
             else if(orden == 2){
                 ActualizarLista(cliente->nick,DESCONECTADO);
                 Sleep(1);
                 ActualizarLista(nuevonick,CONECTADO);
             }
             memcpy(cliente->nick, buffer+2, 31);
             memset(buffer,0,MAXBUFF);          
         }
     }
}

void ActualizarLista(char* usuario, int opcion){
     char tmp[MAXBUFF]={0};
     if (opcion == CONECTADO){
         sprintf(tmp,"-a%s",usuario);
         EnviarAConectados(tmp);
     }
     else if (opcion == DESCONECTADO){
         sprintf(tmp,"-d%s",usuario);
         EnviarAConectados(tmp);
     }
}  

void EnviarLista(_client* cliente){
     for(int i = 0;i<N_DE_CLIENTES; i++){
         if(clientes[i]->connected){
             if(strlen(clientes[i]->nick)){
                 char tmp[MAXBUFF]={0};
                 sprintf(tmp,"-a%s",clientes[i]->nick);
                 Enviar(cliente,tmp);
                 Sleep(1);
             }
         }
     }
}

void Enviar(_client* cliente , char* buffer){
     cliente->x = send(cliente->socket,buffer,MAXBUFF-1,0);
     if (cliente->x == SOCKET_ERROR || cliente ->x == 0){
          Desconectar(cliente);
     }
}

void EnviarAConectados(char* buffer){
    for (short i = 0; i<N_DE_CLIENTES;i++){
        if (clientes[i]->connected == true){
            Enviar(clientes[i],buffer);
        }
    }
}

void Mensaje(char* buffer){
    printf("%s\n",buffer); 
    char mensaje_f[MAXBUFF+20];
    sprintf(mensaje_f,"-m%s",buffer);
    EnviarAConectados(mensaje_f);
}

void Desconectar(_client* cliente){
     SetColor(red);
     closesocket(cliente->socket);
     cliente->connected = false;
     char mensaje_desc[MAXBUFF];
     sprintf(mensaje_desc,"Cliente desconectado: %s (%s)",cliente->nick,cliente->ip);
     Mensaje(mensaje_desc);
     Sleep(2);
     ActualizarLista(cliente->nick,DESCONECTADO);
     memset(&cliente->address , 0 , sizeof(sockaddr));
     memset(&cliente->ip , 0 , 32);
     memset(&cliente->nick , 0 , 32);
}

void Terminar(){
     closesocket(socket_s);
     for (short i = 0;i<N_DE_CLIENTES;i++){
         closesocket(clientes[i]->socket);
     }
     WSACleanup();
     SetColor(red);
     printf("[X]Finalizado con exito.\n");
     system("pause");
}

void Check(){ 
    if (time(NULL) >= tiempo+INTERVALO_CHEQUEO){
		tiempo+=INTERVALO_CHEQUEO;
		EnviarAConectados("c");
	}
}

int main()
{
     SetColor(green);
     char buffer[MAXBUFF] = {0};
     WSADATA wsa;
     bool quit = false;
     WSAStartup(0x0202,&wsa);
     printf("\n\t\t\t\tServidor Non-Block\n\t\t\t\tCreado por Armandito\n");
     if (!Iniciar()){
        Terminar(); return 0 ; }
     printf("[?]Aceptando conexiones...\n\n");
     //Bucle
     while(quit == false){
          Aceptar();
          Recibir(buffer);
          Check();
          memset(buffer,0,MAXBUFF);
          Sleep(1);
          
     }
     return 0;
}
