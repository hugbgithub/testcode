#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX_FD 1024

volatile int max = -1;

int listenfd ;
char buf[1024];
fd_set read_fds;
fd_set exception_fds;

int sel_fd_arr[MAX_FD];

int get_max_fd(){
	int i;
	for (i = MAX_FD-1; i >= 0; i--)
	  if (sel_fd_arr[i] == 0)
	  	return i;
	  	
	return -1;
}

void set_sel_events(){
	int i;
	for (i = 0; i < MAX_FD; i++)
	  if (sel_fd_arr[i] == 0){
	    FD_SET(i, &read_fds);
	    FD_SET(i, &exception_fds);	
	  }
}



void * sel_thread_fun(void * arg){
	
	struct sockaddr_in client_address;
	socklen_t client_addrlength = sizeof(client_address);
	char addr_p[16];
	
	int i = 0;
	printf("sel_thread_fun begin\n");
  while(1){
  	max = get_max_fd();
	  printf("max=%d\n", max);
	  
	  int ret = select(max+1, &read_fds, NULL, &exception_fds, NULL);
	  if (ret < 0){
	    printf("selection failure \n");
	    continue;
	  }  	
	  

	  
	  for (i = 0; i < max+1; i++){
	  	memset(buf, 0x00, sizeof(buf) );
		  if (FD_ISSET(i, &read_fds)){
		  	if (i == listenfd){
				  int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlength);
				  printf("connection from %s \n", inet_ntop(AF_INET, &client_address.sin_addr, addr_p, sizeof(addr_p)));
				  if ( connfd < 0){
				    printf("errno is %d\n", errno);
				    close(listenfd);
				  }
				    
				  sel_fd_arr[connfd] = 0;
		    }
		    else{
			    ret = recv(i, buf, sizeof(buf)-1, 0);
			    if (ret <= 0){
			    	printf("recv ret=%d\n", ret);
			    	sel_fd_arr[i] = -1;
			      break;
			    }
			    printf("get %d bytes of normal data:%s\n", ret, buf);
		    }  
		  }
		  else if( FD_ISSET(i, &exception_fds) ){
		    ret = recv(i, buf, sizeof(buf)-1, MSG_OOB);
		    if (ret <= 0){
		    	sel_fd_arr[i] = -1;
		      break;
		    }
		    printf("get %d bytes of oob data:%s\n", ret, buf);
		  }	  	
	  }
	  
	  set_sel_events();
  	
  }	
	
	return (int*)(1) ;
}
  

int main(int argc, char *argv[]){

  if (argc <= 2){
    printf("usage:%s ip_address port_number \n", argv[0]);
    return 1;
  }
  
  int i;
  for (i = 0; i < MAX_FD; i++)
    sel_fd_arr[i] = -1;
    
  char addr_p[16];

  const char *ip = argv[1];
  int port = atoi(argv[2]);

  int ret = 0;
  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);

  address.sin_port = htons(port);
  listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd >= 0);

  ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address) );
  assert(ret != -1);
  ret = listen(listenfd, 5);
  assert(ret != -1);

  FD_ZERO(&read_fds);
  FD_ZERO(&exception_fds);

  struct sockaddr_in client_address;
  socklen_t client_addrlength = sizeof(client_address);
  
  sel_fd_arr[listenfd] = 0;
  FD_SET(listenfd, &read_fds);
	FD_SET(listenfd, &exception_fds);
	  
  pthread_t thd;
  
  pthread_create(&thd, NULL, sel_thread_fun, NULL);
  
  pthread_join(thd, NULL);
  
  
  /*
  while(1){
	  int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlength);
	  printf("connection from %s \n", inet_ntop(AF_INET, &client_address.sin_addr, addr_p, sizeof(addr_p)));
	  if ( connfd < 0){
	    printf("errno is %d\n", errno);
	    close(listenfd);
	  }
	  if ( connfd > max)
	    max = connfd;
	    
	  FD_SET(connfd, &read_fds);
	  FD_SET(connfd, &exception_fds);
  }
  
  */
  
  close(listenfd);


  return 0;

 

      
}
