#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "players.c"

player p;

static void sighandler(int signo) {
  if (signo == SIGINT) {
    printf("ctrl-c was pressed\n");
    exit(0);
  }
}

int main(int argc, char *argv[]) {

  p = generate_hand(p);
  printf("p.num_cards: %d\n", p.num_cards);
  //player_action(p);
  
  signal(SIGINT, sighandler);

  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
   
  char buffer[256];
   
  if (argc < 3) {
    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    exit(0);
  }
	
  portno = atoi(argv[2]);
  /* Create a socket point */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }
	
  server = gethostbyname(argv[1]);
   
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }
   
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
   
  /* Now connect to the server */
  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
    exit(1);
  }

  while (1) {
    
    player_action(p);
    sleep(1);
    
    /* Now ask for a message from the user, this message
     * will be read by server
     */

    /* Server determines turn */
    char *buffer2;
    //printf("four (r)\n");
    //int a = read(sockfd, buffer2, 255 );
    init *read_mssg = (init*)malloc(sizeof(init));
    printf( "bout to read\n");
    int x = read(sockfd, read_mssg, sizeof(init));
    printf( "i read\n");
    if (x < 0) {
      perror("ERROR reading mssg ");
      printf("error: %s \n", strerror(errno));
      exit(1);
    }
    char *color = stringify_color( read_mssg->top_card );
    printf( "Current card played: [COLOR] %s [VALUE] %d\n", color, read_mssg->top_card.value);
    //printf ( "debug\n" );
    /*printf("able to get pass four\n");
    if (a < 0) {
      perror("ERROR reading go ");
      printf("error: %s \n", strerror(errno));
      exit(1);
    }
    
    printf( "buffer2: %s\n", buffer2 );
    if ( !strcmp(buffer2, "terminate") ) {
      break;
    }
    */
    printf( "mssg: %s\n", read_mssg->mssg );
    if ( !strcmp( read_mssg->mssg, "terminate" ))
      break;
    else if ( !strcmp( read_mssg->mssg, "END"))
      printf("Game over! Someone won.\n");
    //if ( !strcmp(buffer2, "go") ) {
    //read_mssg->mssg = "go";
    else if ( !strcmp(read_mssg->mssg, "go") ) {
      bzero(buffer,256);
      printf("Please enter the message: ");
      fgets(buffer,255,stdin);

      char * scard1;
      char * scard2;
      int num = atoi(buffer);
      
      //printf( "debugging\n");
      if (num==p.num_cards){
        card *write_card = (card*)malloc(sizeof(card));
        //printf( "debugging\n");
	      p.cards[p.num_cards] = draw_card();
	      //printf( "debugging\n");
	      p.num_cards++;
	      //printf( "debugging\n");
	      scard1 = "draw";
	      scard2 = "draw";
	      // SENDING CARD PLAYED BY PLAYER
	      write_card->color = 21;
	      write_card->value = 21;
	      int z = write(sockfd, write_card, sizeof(card) );
	      if (z < 0) {
          perror("ERROR writing");
          printf("error: %s \n", strerror(errno));
          exit(1);
        }
      }
      else{
      	card c = p.cards[num];
      	card *write_card;
      	*write_card = p.cards[num];
      	int value = c.value;
      	int color = c.color;
      	char svalue[100];
      	sprintf(svalue, "%d", value);
      	char cvalue[100];
      	scard1 = svalue;
      	sprintf(cvalue, "%d", color);
      	scard2 = cvalue;
      	if ( c.color == read_mssg->top_card.color || c.value == read_mssg->top_card.value || read_mssg->top_card.color == 20) {
      	  p = remove_card(p, num);
      	  if ( win_scenario(p) == 100 ) {
      	    printf("You won!\n");
      	    write_card->color = 100;
	          write_card->value = 100;
	          int z = write(sockfd, write_card, sizeof(card) );
	          if (z < 0) {
              perror("ERROR writing");
              printf("error: %s \n", strerror(errno));
              exit(1);
            }
      	    exit(0);
      	  }
      	  printf("Successfully placed card!\n");
      	  write_card->color = color;
	        write_card->value = value;
	        int z = write(sockfd, write_card, sizeof(card) );
	        if (z < 0) {
            perror("ERROR writing");
            printf("error: %s \n", strerror(errno));
            exit(1);
          }
      	}
      	else {
      	 printf("Sorry. Invalid card! Draw 1.\n");
      	 p.cards[p.num_cards] = draw_card();
      	 p.num_cards++;
      	 write_card->color = 21;
	       write_card->value = 21;
	       int z = write(sockfd, write_card, sizeof(card) );
	       if (z < 0) {
            perror("ERROR writing");
            printf("error: %s \n", strerror(errno));
            exit(1);
          }
      	}
      	printf("tried to stringify a card in client: %s %s\n", scard2, scard1);
      }

      /* Send message to the server */
      //printf("five (w)\n");
      //n = write(sockfd, buffer, strlen(buffer));
      //n = write(sockfd, scard1, strlen(scard1));
    
    if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
    }
      sleep(1);
      printf( "scard2 %s\n", scard2 );
      //n = write(sockfd, scard2, strlen(scard2));
    }  
    if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
    }
   
    /* Now read server response */
    //bzero(buffer,256);
    //printf("six (r)\n");
    //n = read(sockfd, buffer, 255);
  
    if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
    }
	
    //printf("%s\n",buffer);
    //return 0;
  }
  
}
