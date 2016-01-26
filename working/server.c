#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>  
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "players.c"

int *response;

union semun {
  int val;
  struct semid_ds *buf;    // Buffer for IPC_STAT, IPC_SET
  unsigned short  *array;  // Array for GETALL, SETALL
  struct seminfo  *__buf;  // Buffer for IPC_INFO
};

void sig_handler( int signo ) {
  
  int *array;
  int shm_key, sem_key, shmid, semid;
  shm_key = ftok( "makefile", '2' );
  //sem_key = ftok( "makefile", '1' );
  
  if ( signo == SIGINT ) {
    //printf( "REMOVED SEMAPHORE\n\n" );
    //semid = semget( sem_key, 1, 0644 );
    
    //if ( semctl( semid, 0, IPC_RMID ) == -1 )
    //printf( "[Remove_SEMCTL]Errno: %d Error: %s\n\n", errno, strerror( errno ) );

    printf( "REMOVED SHARED MEMORY\n" );
    shmid = shmget( shm_key, 3 * sizeof(int), 0644 );
    
    if ( shmctl( shmid, IPC_RMID, 0 ) == -1 )
      printf( "[Remove_SHMCTL]Errno: %d Error: %s\n\n", errno, strerror( errno ) );

  }  
  exit(0);
}

void incre_pos() {

  int *array;
  int semid;
  //int semkey = ftok( "makefile", '1' ); //semaphore key
  int shmemkey = ftok("makefile", '2' ); //shared memory key

  //semid = semget(semkey, 1, 0644);
  //if ( semid == -1 )
  //printf("Others are still using shared memory. Waiting...\n");
  /*
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_flg = SEM_UNDO;
    sb.sem_op = -1;

    semop(semid, &sb, 1);
  */
  /* Get the key and attach */
  int check = shmget(shmemkey, 3 * sizeof(int), 0644);
  if(check != -1) {
    array = (int *)shmat(check, 0, 0);
  } 
  else {
    printf( "%d, %s\n", errno, strerror( errno ) );
    printf("Cannot access the shared memory.\n");
    printf("Exited\n");
    exit(0);
  }


  //printf("The previous pos in the shared memory is: %d\n", *current_pos);
  //printf("Enter your line here...\n");
  //fgets(input, 256, stdin);
  
  /* Change the int in the shared memory */
  if ( array[0] < desired_total + 3  )
    array[0] = array[0] + 1;  
  else 
    array[0] = 4;
  
  /* Detach the shared memory*/
  //printf("Your new pos in the shared memory is: %d\n", *current_pos);
  shmdt( (void *)array );

  //sb.sem_op = 1;
  //semop(semid, &sb, 1);

}

void doprocessing (int sock) {
  //printf( "debugging\n" );
  /* Reach into shared memory */
  int *array;
  int shmemkey = ftok("makefile", '2' ); //shared memory key
  //printf( "%d\n", shmemkey );
  int shmid = shmget( shmemkey, 3 * sizeof(int), 0644 );
  array = (int *)shmat( shmid, 0, 0 );
  
  init *write_mssg = (init*)malloc(sizeof(init));
  /*
  write_mssg->top_card.color = array[1];
  write_mssg->top_card.value = array[2];
  int x = write(sock, write_mssg, sizeof(init));
  if (x < 0) {
    perror("ERROR writing mssg ");
    printf("error: %s \n", strerror(errno));
    exit(1);
  }
  printf( "sent mssg\n");
  */
  //printf( "debugging\n" );

  //printf("current_position: %d\n", current_position);
  //printf("desired_total: %d\n", desired_total);

  /* if (current_position == desired_total){ */
  /*   current_position = 0; */
  /* } */

  //printf("sock: %d\n", sock);  
  //printf("player_ids[current_position]: %d\n", player_ids[current_position]);  
  //printf("one (w)\n");
  //if (sock==player_ids[current_position]){
  
  /* Check the pos in shared mem against sockfd */
  sleep(1);
  printf( "sock:%d == %d:mem\n", sock, array[0] );
  if ( array[0] == sock ) {
    //printf( "debugging\n" );
  
    sleep(1); //GO TO SLEEP and wait for read to happen first    
    //int p = write(sock, "go", sizeof("go"));
    printf("bout to set color\n");
    write_mssg->top_card.color = array[1];
    printf("bout to set value\n");
    write_mssg->top_card.value = array[2];
    printf( "bout to set go\n");
    if ( array[1] == 100 ) 
      strcpy(write_mssg->mssg, "END");
    else 
      strcpy(write_mssg->mssg, "go");
    printf( "mssg inside s %s\n", write_mssg->mssg );
    printf("bout to write\n");
    int x = write(sock, write_mssg, sizeof(init));
    if ( array[1] == 100){
      printf("Found winner. Disconnecting...\n");
      exit(0);
    }
    shmdt( (void *)array );
    printf("i wrote\n");
    if (x < 0) {
      perror("ERROR writing topcard ");
      printf("error: %s \n", strerror(errno));
      exit(1);
    }
    printf( "sent topcard\n");
    /*
    if (p < 0) {
      perror("ERROR writing");
      printf("error: %s \n", strerror(errno));
      exit(1);
    }
    */
    int n;
    char buffer[256];
    char buffer3[256];
    bzero(buffer,256);
    sleep(1);
    card *read_card = (card*)malloc(sizeof(card));
    //printf("two (r)\n");
    //n = read(sock,buffer,255);
    n = read(sock, read_card, sizeof(card));
    //printf("able to get pass two\n");
    if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
    }
    //sleep(1);
    //n = read(sock,buffer3,255);
    //if (n < 0) {
    //  perror("ERROR reading from socket");
    //  exit(1);
    //}
  
    //printf("Here is the message: %s %s\n",buffer3, buffer );
    if (read_card->color <= 20)
      printf("Here is the card: color %d value %d\n", read_card->color, read_card->value);
    
    array = (int *)shmat( shmid, 0, 0 );
    if (read_card->color <= 20 || read_card->color == 100 ){
      array[1] = read_card->color;
      array[2] = read_card->value;
      
    //Need to turn buffer and buffer3 into ints
    /*************************************************************
    if ( array[1] == buffer || array[2] == buffer3 ) {
      array[1] = buffer;
      array[2] = buffer3;
    }
    *************************************************************/
      printf( "top color %d top value %d\n", array[1], array[2] );
    }
      
    //array[1] color
    //array[2] value
    /*char cvalue[100];
    sprintf(cvalue, "%d", array[2]);
    char svalue[100];
    sprintf(svalue, "%d", array[1]);	
    //printf("three (w)\n");
    char *topcard = strcat( svalue, ",");
    topcard = strcat( topcard, cvalue );
    printf( "buffer: %s == %s topcard\n", buffer, topcard );
    */
    shmdt( (void *)array );
    //n = write(sock,"I got your message",18);
    
    
    //current_position++;
  
    if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
    }
  
    //printf("current_position: %d\n", current_position);
    //printf("desired_total: %d\n", desired_total);
    incre_pos();
    
  }
  
  //printf( "debugging\n" );
  
}

int main( int argc, char *argv[] ) {
  
  signal( SIGINT, sig_handler );
  
  ask_for_total();
  int turns;
  
  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n, pid;
  
  /* Shared memory */
  
  int shm_key, sem_key, shmid, semid;
  int *array;
  //char *access;
  //struct sembuf sb;
  //sb.sem_flg = SEM_UNDO;
  shm_key = ftok( "makefile", '2' );
  //sem_key = ftok( "makefile", '1' );
  
  if ( shm_key == -1 )
    printf( "[shm_key]Errno: %d Error: %s", errno, strerror( errno ) );
  
  //if ( sem_key == -1 )
  //printf( "[sem_key]Errno: %d Error: %s", errno, strerror( errno ) );
    
  //union semun su;
  printf( "CREATE SHARED MEMORY\n\n" );
  shmid = shmget( shm_key, 3 * sizeof(int), 0644 | IPC_CREAT );

  if ( shmid == -1 )
    printf( "[SHMGET]Errno: %d Error: %s\n", errno, strerror( errno ) );
  
  array = (int *)shmat(shmid, 0, 0);
  //if ( *x == -1 )
  //printf( "[shmat]Errno: %d Error: %s", errno, strerror( errno ) );
  array[0] = 4;
  array[1] = 20; //any color
  array[2] = 20; //any value
  printf( "<server> Set int to %d\n", array[0] );
      
  //printf( "CREATE SEMAPHORE\n\n" );
  //semid = semget( sem_key, 1, 0644 | IPC_CREAT | IPC_EXCL);

  //if ( semid == -1 ) 
  //printf( "[SEMGET]Errno: %d Error: %s\n", errno, strerror( errno ) );

  //su.val = 1;
  //semctl( semid, 0, SETVAL, su );

  // Test incre_pos()
  //incre_pos();
  
  /* First call to socket() function */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }
  
  /* Initialize socket structure */
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = 5001;
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  
  /* Now bind the host address using bind() call.*/
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR on binding");
    exit(1);
  }
  
  /* Now start listening for the clients, here
   * process will go in sleep mode and will wait
   * for the incoming connection
   */
  
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  
  printf("player_count: %d\n", player_count);
  printf("desired_total: %d\n", desired_total);
  
  while ( array[1] != 100 ) {
    if(player_count <= desired_total){
      
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      
      //if ( newsockfd == 4 ) 
      //  sendpos( 4 );
      
      players_connect();

      player_ids[player_count-1] = newsockfd;
      int i=0;
      while(i<player_count){
      	printf("player_ids[%d] = %d\n", i, player_ids[i]);
      	i++;
      }
      
      if (newsockfd < 0) {
	perror("ERROR on accept");
	exit(1);
      }
      
      /* Create child process */
      pid = fork();
            
      /* if ( pid > 0 ){ */
      /* 	child_ids[player_count-2] = pid; */
      /* 	int i2=0; */
      /* 	while(i2<player_count){ */
      /* 	  printf("child_ids[%d] = %d\n", i, child_ids[i]); */
      /* 	  i2++; */
      /* 	} */
      /* 	//printf("getpid: %d\n", getpid()); */
      /* 	printf("pid: %d\n", pid); */
	
      /* 	if (pid < 0) { */
      /* 	  perror("ERROR on fork"); */
      /* 	  exit(1); */
      /* 	} */
      /* } */

      if (pid == 0) {
	
	while( 1 ) {		
	  /* This is the client process */
	  printf("hello\n");
	  //close(sockfd);
	  //printf( "sock:%d\n", newsockfd );
	  doprocessing(newsockfd);

	  /* printf("current_position: %d\n", current_position); */
	  /* printf("desired_total: %d\n", desired_total); */
	  /* printf("player_ids[current_position]: %d\n", player_ids[current_position]); */
	  /* int i=0; */
	  /* while(i<player_count){ */
	  /*   printf("player_ids[%d] = %d\n", i, player_ids[i]); */
	  /*   i++; */
	  /* } */
	  /* printf("current_position: %d\n", current_position); */
	  /* printf("newsockfd: %d\n", newsockfd); */
	  //sleep(1);
	  
	}

      }
    }
    else {
      /* Close the client if desired_total num of players aleady connected */
      init *write_mssg = (init*)malloc(sizeof(init));
      strcpy( write_mssg->mssg, "terminate" ); 
      write(newsockfd, write_mssg, sizeof(init));
      close(newsockfd);
      if (pid==0){
	      close(sockfd);
      }
    }
		
  } /* end of while */
  shmdt( (void *)array );
  return 1;
}
