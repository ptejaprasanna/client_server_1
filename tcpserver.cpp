#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, bind, listen, accept */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <iostream>	/* for cout and cin */
#include <ctime>	/* for srand */
using namespace std;

#define STRING_SIZE 1024

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_TCP_PORT 8339

int main(void) {
   srand(time(0));    //SETS THE SEED
   int sock_server;  /* Socket on which server listens to clients */
   int sock_connection;  /* Socket on which server exchanges data with client */

   struct sockaddr_in server_addr;                /* Internet address structure that
                                                  stores server address */
   unsigned int server_addr_len;                  /* Length of server address structure */
   unsigned short server_port;                    /* Port number used by server (local port) */

   struct sockaddr_in client_addr;                /* Internet address structure that
                                                  stores client address */
   unsigned int client_addr_len;                  /* Length of client address structure */
   unsigned short msg_len = 0;                    /* length of message */
   unsigned short bytes_sent = 0, bytes_recd = 0; /* number of bytes sent or received */
   unsigned short bytes_char_recd = 0, bytes_char_sent = 0, bytes_int_recd = 0;
   unsigned short bytes_int_sent = 0;          //HOLDS THE NUMBER OF INTEGER BYTES SENT
   unsigned short bytes_str_sent = 0;          //HOLDS THE NUMBER OF CHARACTER BYTES SENT
   unsigned short random_gen;                  //SPECIFIES THE NUMBER OF TIMES TO GENERATE RANDOM NUMBERS
   /* VARIABLES THAT HOLD THE NUMBER TRANSMISSIONS (CHARS AND INTS) */
   unsigned short count_transmissions_ints = 0, count_transmissions_chars = 0;
   /* VARIABLES THAT HOLD THE TOTAL NUMBER OF BYTES TRANSMITTED(CHARS AND INTS) */
   unsigned short count_ints = 0, count_chars = 0;
   unsigned short count_letters_n = 0;        //HOLDS THE NUMBER OF CHARACTER IN AN ARRAY
   /* VARIABLE USED FOR CASTING CHAR TO INT */
   unsigned char cast_for_char = 0;
   unsigned char checksum_chars = 0;                //HOLDS THE CHECKUM FOR CHARACTERS
   unsigned short checksum_ints = 0;          //HOLDS THE CHECKSUM FOR INTEGER
   unsigned short count_ints_n = 0;           //HOLDS THE NUMBER OF INTEGERS IN AN ARRAY


   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Server: can't open stream socket");
      exit(1);
   }

   /* initialize server address information */

   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */
   server_port = SERV_TCP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }

   /* listen for incoming requests from clients */

   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   cout << "Server listening on PORT NUMBER " << server_port << endl;

   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */

   for (;;) {

      sock_connection = accept(sock_server, (struct sockaddr *) &client_addr,
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
      if (sock_connection < 0) {
         perror("Server: accept() error\n");
         close(sock_server);
         exit(1);
      }

      /* receive the character and integer */
      char input_char;
      short input_int;

      bytes_char_recd = recv(sock_connection, &input_char, sizeof(input_char), 0);
      bytes_int_recd =  recv(sock_connection, &input_int, sizeof(input_int), 0);
      input_int = ntohs(input_int);
      random_gen = input_int;

      /* ECHO THE CHARACTER AND INTEGER TO THE CLIENT */
      if (bytes_int_recd > 0 && bytes_char_recd > 0){
      /* send message */
         bytes_char_sent = send(sock_connection, &input_char, sizeof(input_char), 0);
         input_int = htons(input_int);
         bytes_int_sent = send(sock_connection, &input_int, sizeof(input_int), 0);
      }

      /***************************************
      *          IF CHARACTER ENTERED IS    *
      *                 C                   *
      ***************************************/

      if(input_char == 'C'){
      /* CREATING A STATIC ARRAY OF CHARACTERS
          rand_chars -> ARRAY FOR EVERY 100 BYTES
          rand_chars_copy -> ARRAY USED FOR SENDING CHARACTERS OVER THE NETWORK
      */
        unsigned char rand_chars[random_gen];
        unsigned char rand_chars_copy[random_gen];

      /* TEMPORARY VARIABLES USED FOR GENERATING RANDOM CHARACTERS */
        char x;
        short n;
      /*                      */

      /* LOOP TO GENERATE RANDOM CHARACTERS */
        for(int i = 0; i < random_gen; i++){
          n = rand() % 26;
          x = (char)(n + 97);
          rand_chars[i] = x;
        }
      /*VARIABLES FOR OBTAINING THE REMAINING BYTES AND TO COUNT THE NUMBER OF CHARACTERS TRANSMITTED */
        unsigned short remainder_char = 0, count_letters_n = 0 ;
      /*VARIABLE TO STORE THE NUMBER OF ITERATIONS */
        short temp = 0;
      /*CONTROL STATEMENT GENERATING NUMBER OF ITERATIONS */
        if ( random_gen >= 100 ){
          temp = (random_gen / 100);           //NUMBER OF ITERATIONS FOR TRANSMITING 100 BYTES
      /* CONTROL STATEMENT TO GENERATE REMAINDER BYTES IF ANY */
          if((random_gen / 100) != 0){
            remainder_char = random_gen % 100;
          }
        }
      /*CONTROL STATEMENT TO OBTAIN THE NUMBER OF BYTES IF LESS THAN 100 BYTES ARE TO BE TRANSMITTED */
        else if ( random_gen < 100 ){
          remainder_char = random_gen % 100;
        }
        int j = 1;                            //VARIABLE USED FOR GENERATING THE REQUIRED NUMBER OF ITERATIONS
        bytes_char_sent = 0;                    //VARIABLE TO HOLD NUMBER OF CHARACTER SENT

      /* LOOP TO SEND 100 BYTES */
        do{
          if(random_gen >= 100){
      /* LOOP THAT COPIES THE REQUIRED 100 BYTES TO BE TRANSMITTED FOR THE SEND OPERATION
          INTO A TEMPORARY ARRAY */
            for(int i = (j - 1 ) * 100, k = 0 ; i < (100 * j), k < 100; i++, k++){
              rand_chars_copy[k] = rand_chars [i];
            }
            bytes_str_sent = send(sock_connection, rand_chars_copy, 100, 0);
            count_chars += bytes_str_sent;       //TO COUNT NUMBER OF BYTES SENT
            count_transmissions_chars++;         //TO HOLD THE NUMBER OF TRANSMISSIONS
      /* RANDOM WAIT */
            unsigned short wait_loop = 0;
            wait_loop = rand() % 999 + 1;
            wait_loop *= 1000;
            while(wait_loop > 0){
              wait_loop--;
            }
      /* END OF WAIT LOOP */
          }

      /* CONTROL STATEMENT IF REMAINDER BYTES ARE TO BE TRANSMITTED */
          if ( j == temp && remainder_char != 0){
      /* LOOP THAT COPIES THE REMAINING BYTES TO BE TRANSMITTED FOR THE SEND OPERATION
          INTO A TEMPORARY ARRAY */
           for(int i = (j ) * 100, k = 0 ; i < (100 * j)+(remainder_char), k < (remainder_char); i++, k++)
           {
             rand_chars_copy[k] = rand_chars [i];
           }
           bytes_str_sent = send(sock_connection, rand_chars_copy, remainder_char, 0);
           count_chars += bytes_str_sent;
           count_transmissions_chars++;
           unsigned short wait_loop = 0;
           wait_loop = rand() % 999 + 1;
           wait_loop *= 1000;
           while(wait_loop > 0){
            wait_loop--;
           }
          }
      /* CONTROL STATEMENT FOR TRANSMITTING CHARACTERS IF LESS THAN 100 BYTES ARE TO BE
          TRANSMITTED INITTIALLY */
          if( random_gen < 100){
      /* LOOP THAT COPIES BYTES (IF < 100) TO BE TRANSMITTED FOR THE SEND OPERATION
          INTO A TEMPORARY ARRAY */
           for(int i = 0; i < remainder_char; i++){
             rand_chars_copy[i] = rand_chars [i];
           }
           bytes_str_sent = send(sock_connection, rand_chars_copy, remainder_char, 0);
           count_chars += bytes_str_sent;
           count_transmissions_chars++;
           unsigned short wait_loop = 0;
           wait_loop = rand() % 999 + 1;
           wait_loop *= 1000;
           while(wait_loop > 0){
            wait_loop--;
           }
          }
          j++;                                    //INCREMENTS AFTER EACH ITERATION
        }while(j <= temp);
        checksum_chars = 0;                       //INITIALIZING CHECKSUM TO ZERO

      /*LOOP TO FIND THE LENGTH OF STRING */
        for ( int i = 0; i < random_gen; i++ ){
          count_letters_n++;
        }
      /*  LOOP TO GENERATE THE CHECKSUM */
        for ( int i = 0; i < count_letters_n; i++){
         cast_for_char = (uint8_t)rand_chars[i];      //CAST TO 8-BIT INT
         checksum_chars +=  cast_for_char;
        }

        cout << endl << "The total number of letters transmitted: " << count_letters_n;
        cout << endl << "the number of separate transmissions (each send counts as a separate transmission): " << count_transmissions_chars;
        cout << endl << "the total number of bytes transmitted (as measured by the sum of the values returned by each send operation): " << count_chars;
        cout << endl << "Checksum: " << (unsigned short)checksum_chars << endl;
      //  printf(" %d ", checksum_chars);

      /* INITIALIZING ALL VARIABLES TO ZERO FOR LATER TRANSMISSIONS */
        count_letters_n = 0;
        count_transmissions_chars = 0;
        count_chars = 0;
        checksum_chars = 0;
      }

      /***************************************
      *          IF ENTERED CHARACTER IS    *
      *                 N                   *
      ***************************************/

      else if(input_char == 'N'){

      /* INTEGER ARRAYS rand_ints_copy -> THIS ARRAY IS TRANSMITTED OVER THE NETWORK
                        rand_ints_temp -> CONTAINS SYSTEM ORDER values
                        rand_ints      -> CONTAINS NETWORK ORDER VALUES       */
          unsigned short rand_ints[random_gen];
          unsigned short rand_ints_temp[random_gen];
          unsigned short rand_ints_copy[random_gen];

      /* LOOP TO GENERATE RANDOM INTEGER NUMBERS */
          for(unsigned int i = 0; i < random_gen; i++){
            rand_ints[i] = rand();
            rand_ints_temp[i] = rand_ints[i];     //MAKING A COPY OF RANDOM ARRAY
            rand_ints[i] = htons(rand_ints[i]);   //CONVERTING TO NETWORK BYTE ORDER
          }
          unsigned int total = random_gen * 2;              //TOTAL NUMBER OF BYTES TO BE TRANSMITTED
          int remainder_int = 0;                  //REMAINING BYTES TO BE TRANSMITTED AFTER N SUBSEQUENT 100 BYTES
      /* VARIABLE TO STORE NUMBER OF ITERATIONS */
          unsigned int temp = 0;
      /* CONTROL STATEMENT TO GENERATE NUMBER OF ITERATIONS */
          if(total >= 100){
            temp = (total / 100);
      /* TO GENERATE REMAINDER BYTES IF ANY AFTER TRANSMISSION OF Nx100 BYTES */
            if((random_gen / 100) != 0){
              remainder_int = total % 100;
            }
          }
      /* CONTROL STATEMENT IF RECV_INT < 100 */
          else if ( total < 100 ){
            remainder_int = total % 100;
          }
          unsigned int j = 1;                 //VARIABLE FOR INCREMENTING NUMBER OF ITERATIONS
          bytes_int_sent = 0;                 //INITIALIZING NUMBER OF INTEGER BYTES SENT TO ZERO

      /* LOOP TO TRANSMIT ALL BYTES */
          do{
            if(total >= 100){
      /* LOOP THAT COPIES THE REQUIRED 100 BYTES TO BE TRANSMITTED FOR THE SEND OPERATION
          INTO A TEMPORARY ARRAY */
              for(int i = (j - 1 ) * 50, k = 0 ; i < (50 * j), k < 50; i++, k++)
              {
                rand_ints_copy[k] = rand_ints [i];
              }
              bytes_int_sent = send(sock_connection, rand_ints_copy, 100, 0);
              count_ints += bytes_int_sent;           //STATEMENT INITIALIZATION TO COUNT THE NUMBER OF BYTES SENT
              count_transmissions_ints++;             //TO COUNT THE NUMBER OF TRANSMISSIONS

      /*RANDOM WAIT */
              unsigned short wait_loop = 0;
              wait_loop = rand() % 999 + 1;
              wait_loop *= 1000;
              while(wait_loop > 0){
                wait_loop--;
              }
      /* END OF RANDOM WAIT */
            }

      /*THIS CONTROL STATEMENT IS EXECUTED IF REMAINDER BYTES ARE TO BE SENT */
            if ( j == temp && remainder_int != 0){
      /* LOOP THAT COPIES THE REMAINING BYTES TO BE TRANSMITTED FOR THE SEND OPERATION
          INTO A TEMPORARY ARRAY */
              for(int i = (j ) * 50, k = 0 ; i < (50 * j)+(remainder_int/2), k < (remainder_int/2); i++, k++)
              {
                rand_ints_copy[k] = rand_ints [i];
              }
              bytes_int_sent = send(sock_connection, rand_ints_copy, remainder_int, 0);
              count_ints += bytes_int_sent;
              count_transmissions_ints++;

      /* RANDOM WAIT*/
              unsigned short wait_loop = 0;
              wait_loop = rand() % 999 + 1;
              wait_loop *= 1000;
              while(wait_loop > 0){
                wait_loop--;
              }
      /* END OF RANDOM WAIT */
            }
      /* CONTROL STATEMENT FOR TRANSMITTING INTEGERS IF RECV_INT < 100 */
            if ( total < 100){

              for(int i = 0; i < (remainder_int/2); i++){
                rand_ints_copy[i] = rand_ints [i];
              }
              bytes_int_sent = send(sock_connection, rand_ints_copy, remainder_int, 0);
              count_ints += bytes_int_sent;
              count_transmissions_ints++;

      /* RANDOM WAIT */
              unsigned short wait_loop = 0;
              wait_loop = rand() % 999 + 1;
              wait_loop *= 1000;
              while(wait_loop > 0){
                wait_loop--;
              }
      /* END OF RANDOM WAIT */
            }
            j++;
          }while(j <= temp);

          checksum_ints = 0;      //INITIALIZING CHECKSUM TO ZERO

      /* STATEMENT BELOW COUNTS NUMBER INTS IN THE ARRAY */
          count_ints_n = (sizeof(rand_ints_temp))/(sizeof(unsigned short));

      /* LOOP TO CALCULATE THE CHECKSUM */
          for (int i = 0; i < count_ints_n; i++){
            checksum_ints +=  rand_ints_temp[i];
          }

          cout << endl << "The total number of integers transmitted: " << count_ints_n;
          cout << endl << "the number of separate transmissions (each send counts as a separate transmission): " << count_transmissions_ints;
          cout << endl << "the total number of bytes transmitted (as measured by the sum of the values returned by each send operation): " << count_ints;
          cout << endl << "Checksum: " << checksum_ints;
          cout << endl;
          count_ints_n = 0;
          count_transmissions_ints = 0;
          count_ints = 0;
          checksum_ints = 0;
        }
      /* SETTING REQUIRED VARIABLES TO ZERO FOR FUTURE REQUESTS FROM THE CLIENT */
        msg_len = 0;
        bytes_sent = 0;
        bytes_recd = 0;
        bytes_char_recd = 0;
        bytes_char_sent = 0;
        bytes_int_recd = 0;
        bytes_int_sent = 0;
        bytes_str_sent = 0;
        random_gen = 0;
        count_transmissions_ints = 0;
        count_transmissions_chars = 0;
        count_ints = 0;
        count_chars = 0;
        count_letters_n = 0;
        cast_for_char = 0;
        checksum_chars = 0;
        checksum_ints = 0;
        count_ints_n = 0;

      /* close the socket */
        close (sock_connection);
   }
}

