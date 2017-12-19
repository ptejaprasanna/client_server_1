#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <iostream>	/* for cout and cin */
#include <ctime>	/* for srand       */
using namespace std;

#define STRING_SIZE 1024

int main(void){

   srand(time(0));                    /* SETS THE SEED */
   int sock_client;                   /* Socket used by client */
   struct sockaddr_in server_addr;    /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;        /* Structure to store server's IP
                                        address */
   char server_hostname[STRING_SIZE]; /* Server's hostname */
   unsigned short server_port;        /* Port number used by server (remote port) */
   char input_char;                   //C OR N CHARACTER THAT IS SENT TO THE SERVER
   unsigned short bytes_char_sent, bytes_int_sent, bytes_char_recd, bytes_int_recd; /* number of bytes sent or received */
   unsigned short input_int;          //INTEGER THAT IS ENTERED BY THE USER WHEN ASKED PROMPTED BY THE CLIENT
   char recv_char;                    //CHARACTER WHICH IS ECHOED BY THE SERVER
   unsigned short recv_int;           //INTEGER WHICH IS ECHOED BY THE SERVER
   unsigned short bytes_str_recv;     //HOLDS THE NUMBER OF CHARACTER BYTES RECEUVED
   unsigned short bytes_int_recv;     //HOLDS THE NUMBER OF INTEGER BYTES RECEIVED
   unsigned char checksum_chars = 0;        //HOLDS THE CHECKSUM FOR RECEIVED CHARACTERS
   unsigned short checksum_ints = 0;  //HOLDS THE CHECKSUM FOR RECEIVED INTEGERS
   /* VARIABLES TO COMPUTE NUMBER OF RECEIVE TRANSMISSIONS(CHARS AND INTS) */
   unsigned short count_receive_chars = 0, count_receive_ints = 0;
   /* VARIABLES TO COMPUTE NUMBER OF CHARACTERS AND INTEGERS IN THE RECEIVED ARRAY */
   unsigned short count_letters = 0, count_ints_n = 0;
   unsigned char cast_for_char;             //VARIABLE USED FOR CASTING CHAR TO INT
   char q;                            //CHARACTER USED TO KNOW IF THE USER WANTS TO START PROGRAM AGAIN

   /* LOOP TO CONTINUE THE PROCESS IF THE USER WANTS TO BEGIN THE PROCESS AGAIN */
   do{
  /* open a socket */
    if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
    }

   /* Note: there is no need to initialize local client address information
            unless you want to specify a specific local port
            (in which case, do it the same way as in udpclient.c).
            The local address initialization and binding is done automatically
            when the connect function is called later, if the socket has not
            already been bound. */

   /* initialize server address information */

    cout << "Enter hostname of server: ";
    cin >> server_hostname;
    if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname");
      close(sock_client);
      exit(1);
    }

    cout << "Enter port number for server: ";
    cin >> server_port;

   /* Clear server address structure and initialize with server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy((char *)&server_addr.sin_addr, server_hp->h_addr, server_hp->h_length);
    server_addr.sin_port = htons(server_port);

    /* connect to the server */

    if (connect(sock_client, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
    }

   /* user interface */

   /* LOOP TO PROMPT THE USER TO INPUT AGAIN IF WRONG INPUT IS ENTERED */
    do{
      cout << "Input a character (C or N):\n";
      cin >> input_char;
      if(input_char == 'C' || input_char == 'N'){
        cout << "Please enter an integer: ";
        cin >> input_int;
        input_int = htons(input_int);
        break;
        }

      else{
        cout << "\nWrong input entered (C and N are the only inputs you can enter!)";
        cout << "\nEnter a character again. "<<endl;
        }
      }while(input_char != 'C' || input_char != 'N');

   /* send message */

      bytes_char_sent = send(sock_client, &input_char, sizeof(input_char), 0);
      bytes_int_sent =  send(sock_client, &input_int, sizeof(input_int),0);

   /* get response from server */

      bytes_char_recd = recv(sock_client, &recv_char, sizeof(recv_char), 0);
      bytes_int_recd = recv(sock_client, &recv_int, sizeof(recv_int), 0);
      recv_int = ntohs(recv_int);

      cout << "\nThe response from the server is:\n";
      cout << "Character entered: " << recv_char << "  Interger entered: " << recv_int << endl;

    /* INITIALIZING ALL VARIABLES TO ZERO */

      bytes_str_recv = 0;
      bytes_int_recv = 0;
      checksum_chars = 0;
      checksum_ints = 0;
      count_receive_chars = 0;
      count_receive_ints = 0;
      count_letters = 0;
      count_ints_n = 0;
      cast_for_char = 0;

   /***************************************
   *          IF CHARACTER ENTERED IS    *
   *                 C                   *
   ***************************************/

      if(recv_char == 'C'){

    /* STATIC ARRAYS TO STORE CHARACTERS */
        unsigned char recv_str[recv_int];
        unsigned char recv_str_rem [recv_int];
        unsigned char str_eval[recv_int];
        int remainder_char = 0, count_chars_n = 0;   //VARIABLES TO STORE REMAINDER BYTES AND NUMBER OF RECEIVED CHARS RESPECTIVELY
        int temp = 0;                                //VARIABLE TO STORE NUMBER OF ITERATIONS
        count_receive_chars = 0;                     //VARIABLE TO STORE NUMBER OF RECEIVE OPERATIONS
        bytes_str_recv = 0;                           //INITIALIZING NUMBER OF BYTES RECEIVED TO ZERO
    /* CONTROL STATEMENT TO GENERATE NUMBER OF ITERATIONS */
        if( recv_int >= 80 ){
          temp = (recv_int / 80);
    /* CONTROL STATEMENT TO GENERATE REMAINDER BYTES */
          if((recv_int / 80) != 0){
            remainder_char = recv_int % 80;
            }
        }
    /* IF NUMBER OF BYTES ARE LESS THAN 80 */
        else{
          remainder_char = recv_int % 80;
        }
        int j = 1;                                   //INITIALIZING ITERATION VARIABLE

    /* LOOP TO RECEIVE 80 SEPERATE BYTES AND REMAINDER BYTES IF LESS THAN 80 IF ANY */
        do{
          if(recv_int >= 80 ){
            bytes_str_recv = recv(sock_client, recv_str, 80, MSG_WAITALL);

    /* LOOP TO STORE RECEIVED OF CHARS INTO AN ANOTHER ARRAY OF CHARS WHICH
         WOULD CONTAIN ALL RECEIVED CHARS */
            for(int i = (j - 1) * 80, k = 0 ; i < (80 * j), k < 80; i++, k++){
              str_eval[i] = recv_str[k];
              }
            count_receive_chars++;                    //VARIABLE TO HOLD NUMBER OF RECEIVE OPERATIONS
            count_chars_n += bytes_str_recv;          //TO COUNT THE NUMBER OF BYTES RECEIVED

    /* RANDOM WAIT   */
            short wait_loop = 0;
            wait_loop = rand() % 999 + 1;
            wait_loop *= 1000;
            while(wait_loop > 0){
              wait_loop--;
            }
    /* END OF RANDOM WAIT */
          }

    /* CONTROL STATEMENT FOR RECEIVING REMAINDER BYTES IF ANY */
          if ( j == temp && remainder_char != 0 ){
            bytes_str_recv = recv(sock_client, recv_str_rem, remainder_char, MSG_WAITALL);
            for(int i = (j * 80), k = 0 ; i < ((80 * j) + remainder_char), k < remainder_char; i++, k++){
              str_eval[i] = recv_str_rem[k];
            }
            count_chars_n += bytes_str_recv;
            count_receive_chars++;
    /* RANDOM WAIT */
            short wait_loop = 0;
            wait_loop = rand() % 999 + 1;
            wait_loop *= 1000;
            while(wait_loop > 0){
              wait_loop--;
            }
    /* END OF RANDOM WAIT */
          }

    /* CONTROL STATEMENT IF INITIAL NUMBER OF BYTES ARE LESS THAN 80 */
          if(recv_int < 80){
            bytes_str_recv = recv(sock_client, recv_str_rem, remainder_char, MSG_WAITALL);
            for(int i = 0; i < remainder_char; i++){
              str_eval[i] = recv_str_rem[i];
            }
            count_chars_n += bytes_str_recv;
            count_receive_chars++;
    /* RANDOM WAIT */
            unsigned short wait_loop = 0;
            wait_loop = rand() % 999 + 1;
            wait_loop *= 1000;
            while(wait_loop > 0){
              wait_loop--;
            }
          }
    /* END OF RANDOM WAIT */
          j++;                                //INCREMENTS AFTER EACH ITERATION
        }while( j <= temp);
    /* END OF RECEIVE LOOP */

    /* COUNTS THE NUMBER OF CHARACTERS IN THE ARRAY */
        for ( int i = 0; i < recv_int; i++){
          count_letters++;
        }

    /* LOOP TO EVALUATE THE CHECKSUM OF RECEIVED CHARS */
        for ( int i = 0; i < count_letters; i++){
         cast_for_char = (uint8_t)str_eval[i];      //CAST TO 8-BIT INT
         checksum_chars +=  cast_for_char;
        }
        cout << endl << "The total number of letters received: " << count_letters;
        cout << endl << "the number of separate receive operations: " << count_receive_chars;
        cout << endl << "the total number of bytes received (as measured by the sum of the values returned by each receive operation): " << count_chars_n;
        cout << endl << "Checksum: " << (unsigned short)checksum_chars;

    /* INITIALIZING VARIABLES TO ZERO FOR NEXT RECEIVE OPERATION IF NEEDED */
        count_letters = 0;
        count_receive_chars = 0;
        count_chars_n = 0;
        checksum_chars = 0;
      }
    /* END OF CONTROL STATEMENT IF ENTERED CHARACTER IS C */

   /***************************************
   *          IF CHARACTER ENTERED IS     *
   *                 N                    *
   ***************************************/

      else if ( recv_char == 'N'){
        unsigned short array[recv_int];         //ARRAY FOR N x 100 BYTES
        unsigned short array1[recv_int];        //ARRAY1 IS THE COPY OF ARRAY
        unsigned short array2[recv_int];        //ARRAY FOR REMAINING BYTES (<100) IF ANY

    /* INITIALIZING THE ARRAYS TO 0 */
        for(unsigned int i = 0; i < recv_int; i++){
          array[i] = 0;
          array2[i] = 0;
          array1[i] = 0;
        }
        unsigned int total = recv_int * 2;  //TOTAL NUMBER OF BYTES FOR THE RECEING SEQUENCE (2 BYTES FOR EACH SHORT INT)
        int remainder_int = 0;              //REMAINDER BYTES AFTER N SUBSEQUENT 80 BYTES
        unsigned short count_ints = 0;        //TO COUNT THE NUMBER OF RECEIVED INTEGERS
        count_receive_ints = 0;             //TO COUNT THE NUMBER OF RECEIVE OPERATIONS
        bytes_int_recv = 0;
        unsigned short temp = 0;            //SETTING NUMBER OF RECEIVED BYTES TO ZERO

    /* CONTROL STATEMENT TO GENERATE NUMBER OF ITERATIONS IF NUMBER OF BYTES
          TO BE RECEIVED ARE EQUAL TO OR MORE THAN 80 */
        if( total >= 80 ){
          temp = (total / 80);              //VARIABLE TO STORE NUMBER OF RECEIVE ITERATIONS

    /* CONTROL STATEMENT TO GENERATE REMAINDER BYTES IF ANY */
          if((recv_int / 80) != 0){
            remainder_int = total % 80;
          }
        }
    /* IF NUMBER OF BYTES TO BE RECEIVED ARE LESS THAN 80 */
        else{
          remainder_int = total % 80;
        }
        unsigned int j = 1;                  //VARIABLE USED FOR ITERATING

    /* LOOP FOR RECEIVING 80 SEPERATE BYTES IN EACH ITERATION */
        do{

    /* CONTROL STATEMENT IF NUMBER OF BYTES TO BE RECEIVED ARE MORE THAN OR EQUAL TO 80 */
          if( total >= 80){
            bytes_int_recv = recv(sock_client, array, 80, MSG_WAITALL);

    /* LOOP TO CONVERT RECEIVED INTEGERS TO SYSTEM ORDER BYTES */
          for(int i = 0; i < 40; i++){
          array[i]=ntohs(array[i]);
          }
    /*END OF LOOP*/

    /* LOOP TO COPY RECEIVED ARRAY TO FINAL ARRAY */
          for(unsigned int i = (j - 1 ) * 40, k = 0 ; i < (40 * j), k < 40; i++, k++){
          array1[i] = array [k];
          }
    /* END OF LOOP */

          count_receive_ints++;              // STORES NUMBER OF RECEIVE OPERATIONS
          count_ints = count_ints + bytes_int_recv;      // STORES NUMBER OF RECEIVED BYTES
    /* WAIT TIME */
          unsigned short wait_loop = 0;
          wait_loop = rand() % 999 + 1;
          wait_loop *= 1000;
          while(wait_loop > 0){
            wait_loop--;
          }
    /* END OF TIME LOOP */
          }

    /* CONTROL STATEMENT TO RECEIVE REMAINDER BYTES IF ANY */
          if(j == temp && remainder_int != 0){
            bytes_int_recv = recv(sock_client, array2, remainder_int, MSG_WAITALL);

    /* LOOP TO CONVERT ARRAY TO SYSTEM ORDER BYTES */
          for(int i = 0 ; i < (remainder_int/2); i++){
           array2[i]=ntohs(array2[i]);
          }
    /* END OF LOOP */

    /* LOOP TO COPY REMAINDER ARRAY TO THE FINAL ARRAY */
          for(int i = (j * 40), k = 0 ; i < ((40 * j)+(remainder_int/2)), k < (remainder_int/2); i++, k++)
          {
           array1[i] = array2 [k];
          }
    /* END OF LOOP */

          count_ints = count_ints + bytes_int_recv;     //STORES THE NUMBER OF RECEIVED BYTES
          count_receive_ints++;             //INCREMENTS RECEIVE OPERATIONS
    /* WAIT TIME */
          unsigned short wait_loop = 0;
          wait_loop = rand() % 999 + 1;
          wait_loop *= 1000;
          while(wait_loop > 0){
            wait_loop--;
          }
    /* END OF TIME LOOP */
          }
    /* CONTROL STATEMENT IF NUMBER OF BYTES TO BE RECEIVED ARE LESS THAN 80 */
          if( total < 80 ){
            bytes_int_recv = recv(sock_client, array2, remainder_int, MSG_WAITALL);

    /* LOOP TO CONVERT ARRAY TO SYSTEM ORDER BYTES */
          for(int i = 0 ; i< (remainder_int/2); i++){
           array2[i]=ntohs(array2[i]);
          }
    /* END OF LOOP */

    /* LOOP TO COPY REMAINDER ARRAY TO THE FINAL ARRAY */
          for(int i = 0; i < (remainder_int/2); i++)
          {
           array1[i] = array2[i];
          }
    /* END OF LOOP */

          count_ints = count_ints + bytes_int_recv;     //STORES THE NUMBER OF RECEIVED BYTES
          count_receive_ints++;                         //INCREMENTS RECEIVE OPERATIONS
    /* WAIT TIME */
          unsigned short wait_loop = 0;
          wait_loop = rand() % 999 + 1;
          wait_loop *= 1000;
          while(wait_loop > 0){
            wait_loop--;
          }
    /* END OF TIME LOOP */
        }

    /* END OF TIME LOOP */
      j++;                       //INCREMENT AFTER ITERATION
    }while(j <= temp);

    /* To count the number of integers received */
    count_ints_n = (sizeof(array1))/(sizeof(unsigned short));

    /* LOOP TO EVALUATE CHECKSUM FOR RECEIVED INTEGERS */
    for ( int i = 0; i < count_ints_n ; i++){
        checksum_ints +=  array1[i];
    }
    /* END OF LOOP */

    cout << endl << "The total number of integers received: " << count_ints_n;
    cout << endl << "the number of separate receive operations: " << count_receive_ints;
    cout << endl << "the total number of bytes received (as measured by the sum of the values returned by each receive operation): " << count_ints;
    cout << endl << "Checksum: " << checksum_ints;

    /* SETS FOLLOWING VARIABLES TO ZERO FOR NEXT RECEIVE OPERATION IF ANY */
    count_ints_n = 0;
    count_receive_ints = 0;
    count_ints = 0;
    checksum_ints = 0;
    }
    cout << endl <<"Do you want to continue?( y | n ) ";
    cin >> q;

    /* CONTROL STATEMENT IF THE USER WANTS TO CONTINUE WITH THE PROGRAM */
    if(q == 'y'){
    cout << "Continuing" << endl;
    }
    else
    break;

  }while(q == 'y');
 /* close the socket */
 close (sock_client);

}

