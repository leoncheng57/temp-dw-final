# Uno
###### Project by Andy Tso, Eric He, Leon Cheng (Period 8)

### How to compile:
    $ make

### How to use:
1. One computer creates a server using the command
    $ ./server
2. Then choose the number of players
3. Another player joins the game on a separate computer using the command
    ```
    $ ./client <IP address of computer hosting server> 5001
    ```    
    Note: This seems to work only across computers connected on the same network
4. After being connected, there are self-explanatory instructions

Some notes:

1. If you play an invalid card or give an invalid input, your turn will be
   skipped and you will draw a card instead. This is to improve the overall
   experience for other players who must wait for you to make a move.

2. 

List of Files:
- client.c
- server.c
- players.c
- makefile

<br>

- README.txt
- BUGS.txt
- TODO.txt
- DESIGN.txt
