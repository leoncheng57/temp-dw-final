#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

typedef struct
{
  int color; //0-red, 1-blue, 2-green, 3-yellow
  int value; //10-skip, 11-reverse, 12-plus two, 13-wild, 14-wild four
} card;

typedef struct
{
  //int position;
  card cards[100];
  int num_cards;//number of cards in player's hand
} player;

typedef struct
{
  card top_card;
  char mssg[100];
} init;


int desired_total;
int player_count;
int player_ids[100];

int current_player;
player player_list[1000];
card top_card;

/*
  List of functions:
  draw_card() - returns card of random value and random color
  play_card(player p, card c) - check if card is valid, remove card from 
  player p's hand, change top card (uses the update_top_card() function)
  update_top_card( card c ) - changes top_card to c
  player_action() - prints options player can take, gets player input  
*/

int win_scenario(player p) {
  if (p.num_cards == 0)
    return 100;
  return 0;
}

card draw_card() {
  card new_card;

  int fd1;
  fd1 = open("/dev/urandom",O_RDONLY);

  int i = 0;
  int num = 0;
  for(;i<2;i++){
    read(fd1,&num,sizeof(num));
    //printf("Random %d: %d \n",i,num);
  }
  close(fd1);

  num = abs(num);
  
  new_card.color = num % 4;//[0,3]
  new_card.value = num % 15;//[0,14]
  return new_card;
}

void update_top_card( card c ) {
  top_card.color = c.color;
  top_card.value = c.value;
}

char *stringify_color(card c) { // NOT SURE IF STRING SYNTAX CORRECT HERE
  char *color;
  if (c.color == 0) 
    color = "red";
  else if (c.color == 1) 
    color = "blue";
  else if (c.color == 2) 
    color = "green";
  else if (c.color == 3)
    color = "yellow";
  else 
    color = "any color";
  return color;
}

char *stringify_value(card c) { // NOT SURE IF STRING SYNTAX CORRECT HERE
  char *value;
  if (c.value < 10) { //is a number card
    //char snum[5];
    //itoa(c.value, snum, 5);
    char str[100];
    sprintf(str, "%d", c.value);
    value = str;
  }
  else { //is an action card
    if (c.value == 10)
      value = "skip";
    else if (c.value == 11)
      value = "reverse";
    else if (c.value == 12) 
      value = "draw 2";
    else if (c.value == 13) 
      value = "wild card";
    else if (c.value == 14) 
      value = "wild draw 4";
  }
  return value;
}

char * stringify_card (card c){
  char * ret = strcat(stringify_value(c),stringify_color(c));    
  return ret;
}

player remove_card(player p, int i) {
  card ret;
  ret.color = p.cards[i].color;
  ret.value = p.cards[i].value;
  p.cards[i] = p.cards[p.num_cards-1];
  card nll;
  p.cards[p.num_cards-1] = nll;
  p.num_cards--;
  return p;
}
/*
void play_card(player p, int i) {
  //check if this is a valid next card to play
  if ( p.cards[i].color == top_card.color || p.cards[i].value == top_card.value ) {
    //remove card from p.cards and update num_cards
    p.num_cards--;
    remove_card( p, i );
    update_top_card(  );
    //**[DONE]**change the card that is on top of the pile (shared memory?)
  } 
  else {
    //ask player for another card to play
    printf( "Invalid card \n" ); 
    //play_card(p, newcard);
  }
}
*/
void player_action(player p) {
  //print out options for the player
  printf("It's your turn! What would you like to do?\n");
  printf("Options:\n");
  int i;
  printf("p.num_cards: %d\n", p.num_cards);
  for ( i = 0; i < p.num_cards; i++ ) {
    //if ( p.cards[i].color == 0 )
    printf("%d - play %s %s\n", i, stringify_color(p.cards[i]), stringify_value(p.cards[i]));
  }
  printf("%d - draw a card\n", i++);

  /*
  //get player input as an int
  int input;
  scanf("%d", &input);
  //action
  if (input < p.num_cards && input >= 0) { //player wanted to play a card
  //code to remove card from hand, update top_card, update num_cards
  play_card( p, input );
    
  }
  else if (input == p.num_cards) { //player wants to draw a card
  //code to draw a card and update num_cards
  p.cards[p.num_cards] = draw_card(); 
  p.num_cards++;
  //skip, reverse, +2, wild, wild +4
  }
  
  else { //player entered an invalid input
  //ask player to input a valid input
  print( "Invalid input\n");
  }
  //next_player();
  */
}

//Note: Skip and Reverse are to be dealt with later
/*
  void next_player(){
  player players[ desired_total ];

  if ( current_player >= desired_total )
  current_player = 0;
  else
  current_player++;
  }

  void add_player(player p){
  player_list[ current_player ] = p;
  }
*/



player generate_hand(player p){
  for (p.num_cards = 0; p.num_cards<7; p.num_cards++){
    p.cards[p.num_cards] = draw_card();
    //printf("p.cards[p.num_cards].value: %d \n", p.cards[p.num_cards].value);
    //printf("p.cards[p.num_cards].color: %d \n", p.cards[p.num_cards].color);
    //printf("p.num_cards: %d\n", p.num_cards);
  }
  return p;
}


void ask_for_total(){
  player_count = 0;
  printf("<server> How many players? ");
  scanf ("%d",&desired_total);
  printf("desired_total: %d\n", desired_total);
}

void players_connect(){
  player_count++;
  if (player_count==desired_total){
    printf("Desired total has been reached!\n\n");
  }
  printf("a player has joined\n");
  printf("players connected: %d\n", player_count);
}
