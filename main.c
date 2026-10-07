#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdint.h>
#include<string.h>
typedef struct server{
	int server_fd ; 
	uint16_t port ; 

}__server_t ; 


__server_t *server_setup(uint16_t port){
	__server_t *s = (__server_t*)malloc(sizeof(__server_t)) ; 
	
	if(!s){
		perror("Malloc error");
		return NULL ; 
	}

	int server_fd = socket(AF_INET , SOCK_STREAM  , 0) ;
	
	if(server_fd == -1) {
		free(s) ; 
		perror("Socket init error");
		return NULL  ; 
	}
	
	int opt = 1;
   	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in server_addr ;
	server_addr.sin_family = AF_INET ;
	server_addr.sin_port = htons(port) ;
	server_addr.sin_addr.s_addr = INADDR_ANY ;
	
	s->server_fd = server_fd ;
	s->port = port ; 

	if(bind(server_fd , (struct sockaddr*)&server_addr , sizeof(server_addr)) != 0){
		free(s);
		close(server_fd); 
		perror("Binding Error");
		return NULL ; 
	}
	
	

	if(listen(server_fd , 10) == -1){
		free(s) ;
		close(server_fd); 
		perror("Listening Error");
		return NULL ; 
	}
	
	return s ; 	

}

void process_request(int client_fd){
	char buffer[4096] ; 
	
	int bytes =  recv(client_fd , buffer , sizeof(buffer) ,  0);
		

		

	printf("%s\n" , buffer) ; 
	
	char *response =  "HTTP/1.1 200 OK\r\n"
    			"Content-Type: text/plain\r\n"
    			"Content-Length: 13\r\n"
    			"\r\n"
    			"Hello, World!"; 

	send(client_fd , response , strlen(response) , 0) ;

}


int main(){
	
	uint16_t port = 8000 ; 
	__server_t *s = server_setup(port) ;

	if(!s){
		fprintf(stderr , "Error setup the server") ; 
		return -1 ; 
	}
	
	printf("Listening on port %d\n" , port );

	
	while(1){
		struct sockaddr_in client_addr ;
		socklen_t client_len = sizeof(client_addr) ; 
		int client_fd = accept(s->server_fd ,(struct sockaddr*)&client_addr , &client_len );  
		
		process_request(client_fd) ; 
		
		close(client_fd);
	}

	close(s->server_fd) ; 

}
