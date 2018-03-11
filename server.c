#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include </usr/local/include/mysql/mysql.h>
#include </usr/local/include/mysql/my_global.h>

void finish_with_error(MYSQL *con);

typedef struct 
{
    pthread_t thread_tid;      /* thread ID */
    long    thread_count;      
 }  Thread;

int login_signup(int newsockfd);

/*
 *  sockfd is the socket descriptor which is an integer value
 *  client_address is the struct used to specify a local or remote endpoint address of client.
 */
Thread *tptr;                  
socklen_t addrlen;
pthread_mutex_t mlock;
void thread(int);
struct sockaddr_in serv_addr;
int sockfd, i, nthreads;
int address_len = sizeof(serv_addr);
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{    
    nthreads = atoi(argv[argc - 1]);
    tptr = calloc(nthreads, sizeof(Thread));
    
    /*  Socket() call takes three arguments:
    *  The family of protocol used/address family
    *  Type of socket
    *  Protocol number or 0 for a given family and type
        */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    /*
     *  Socket call will return a socket descriptor on success which is an integer
     *  And it will return '-1' for error
     */
    if (sockfd == -1)
    {
        printf("Error calling Socket\n");
        exit(1);
    }
    /* Populating the sockaddr_in struct with the following values */
    /* Assigning the AF_INET (Internet Protocol v4) address family */
    serv_addr.sin_family = AF_INET;
    /* Populating the Server IP address with the value of the localhost IP address */
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* Converting the port number received from the command line from host byte order to network byte order */
    serv_addr.sin_port = htons(8000);

    /* to set the socket options- to reuse the given port multiple times */
    int num=1;
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,(const char*)&num,sizeof(num))<0)
    {
        printf("setsockopt(SO_REUSEPORT) failed\n");
        exit(0);
    }
    
    /*
     *  Bind takes three arguments: - Used to bind the local endpoint parameters to the socket
     *  Socket descriptor
     *  Server Address Structure - Local endpoint in this case
     *  Size of the address
     */
    /* Returns 0 for success and -1 for failure */
    if(bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("Error binding\n");
        exit(1);
    }
    
    /* Listen call makes the socket to be a passive type, taking two arguments:
     *  Socket Descriptor
     *  number of connection that can be queued
     */
    if(listen(sockfd,5) < 0)
    {
        printf("Error listening\n");
        exit(1);
    }
    
    /* nthreads number of threads are created as specified on command line */
    for(i=0; i<nthreads; i++)
    {
        thread(i);
    }
    for ( ; ; )       
        pause();   
}             

void thread(int i)
{
    void *thread_main(void *);
    /* new threads are created to handle file transfer using pthread_create(0 call */
    pthread_create(&tptr[i].thread_tid, NULL, &thread_main, (void *)(uintptr_t) i);
    return;
}

void *thread_main(void *arg)
{
     int     newsockfd;
     void    web_child(int);
     printf("Thread %d starting\n", (int) arg);
     for ( ; ; ) 
     {
         
        pthread_mutex_lock(&mlock);
         
         
         
        /* server accepts incoming conenction on sockfd and assigns it to newsockfd */
        /*   Accept takes three arguments:
          *  Socket descriptor
          *  Server Address Structure - Local endpoint in this case
          *  Size of the address
        */
        newsockfd = accept(sockfd, (struct sockaddr *) &serv_addr,(socklen_t*)&address_len);
        pthread_mutex_unlock(&mlock);
        tptr[(int) arg].thread_count++;
        
        login_signup(newsockfd);
        
        close(newsockfd);
    }
}

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}


int login_signup(int newsockfd)
{
    //Connecting to database.....
    MYSQL *con = mysql_init(NULL);
    if (con == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }
    
    if (mysql_real_connect(con, "localhost", "root", "root","CMPE_220", 0, NULL, 0) == NULL)
    {
        finish_with_error(con);
    }
    //........................
    
    char LoginDetails[20];
    char signupdetails[500];
    int flag;
    recv(newsockfd, &flag, sizeof(int), 0);
    
    if(flag == 1)
    {
        recv(newsockfd,LoginDetails,sizeof(LoginDetails),0);
        char *LoginID = strtok(LoginDetails," ");
        char *Password = strtok(NULL,"\n");
        printf("\nLoginID: %s\nPassword: %s\n",LoginID,Password);
        
        char query[300];
        sprintf(query,"SELECT Role FROM Login WHERE Login_ID = '%s' AND Password = '%s'",LoginID,Password);
        if (mysql_query(con, query))
        {
            printf("\nin query\n");
            finish_with_error(con);
        }
        
        MYSQL_RES *result = mysql_store_result(con);
        if (result == NULL)
        {
            finish_with_error(con);
        }
        
        int num_fields = mysql_num_fields(result);
        //printf("Fields-%d\n",num_fields);
        MYSQL_ROW row;
        char* send_buf[10];
        int count = 0;
        while ((row = mysql_fetch_row(result)))
        {
            count = 1;
            for(int i = 0; i < num_fields; i++)
            {
                send_buf[i] = row[i];
                printf("%s",row[i]);
                printf("\n%s LOGIN AUTHENTICATED\n", send_buf[i]);
                send(newsockfd, send_buf[i], sizeof(send_buf), 0);
            }
        }
        if (count == 0)
        {
            char message[] = "INVALID";
            send(newsockfd,message,sizeof(message),0);
            login_signup(newsockfd);
        }
        char option[5];
        while(recv(newsockfd,option,sizeof(option),0)>0)
        {
            printf("\nOption received - %s for user - %s\n", option, LoginID);
            if(strcmp(send_buf[0],"CUSTOMER") == 0)
            {
                if(strcmp(option,"1") == 0)
                {
                    char query[300];
                    sprintf(query,"SELECT * FROM LOGIN WHERE Login_ID = '%s'",LoginID);
                    if (mysql_query(con, query))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    
                    MYSQL_RES *result = mysql_store_result(con);
                    if (result == NULL)
                    {
                        finish_with_error(con);
                    }
                    
                    int num_fields = mysql_num_fields(result);
                    MYSQL_ROW row;
                    char* send_buf[10];
                    char send_buffer[100];
                    bzero(send_buffer,sizeof(send_buffer));
                    printf("Login_ID\tEmail_ID\tPassword\tRole\t\tFull_Name\tPhone_Number\tAddress\n");
                    while ((row = mysql_fetch_row(result)))
                    {
                        for(int i = 0; i < num_fields; i++)
                        {
                            send_buf[i] = row[i];
                            printf("%s\t",send_buf[i]);
                            if(i != 0)
                            {
                                strcat(send_buffer,"\t");
                            }
                            if(i == 1)
                            {
                                strcat(send_buffer,"\t");
                            }
                            strcat(send_buffer,send_buf[i]);
                        }
                        send(newsockfd, send_buffer, sizeof(send_buffer), 0);
                        memset(send_buf, 0, sizeof(send_buf));
                        memset(&send_buffer[0], 0, sizeof(send_buffer));
                        //char send_buffer[100] = {0};
                        //bzero(send_buffer,100);
                        printf("\n");
                    }
                    //mysql_close(con);
                }
                
                if(strcmp(option,"2") == 0)
                {
                    char input[200];
                    char out[]="Profile has been updated";
                    char id[5];
                    recv(newsockfd,id,sizeof(id),0);
                    recv(newsockfd,input,sizeof(input),0);
                    printf("Received -\t%s",input);
                    if(strcmp(id,"1") == 0)
                    {
                        sprintf(query,"UPDATE Login SET Email_ID='%s' WHERE Login_ID = '%s'",input,LoginID);
                        if (mysql_query(con, query))
                        {
                            printf("\nin query\n");
                            finish_with_error(con);
                        }
                    }
                    if(strcmp(id,"2") == 0)
                    {
                        sprintf(query,"UPDATE Login SET Password='%s' WHERE Login_ID = '%s'",input,LoginID);
                        if (mysql_query(con, query))
                        {
                            printf("\nin query\n");
                            finish_with_error(con);
                        }
                    }
                    if(strcmp(id,"3") == 0)
                    {
                        sprintf(query,"UPDATE Login SET Phone_Number='%s' WHERE Login_ID = '%s'",input,LoginID);
                        if (mysql_query(con, query))
                        {
                            printf("\nin query\n");
                            finish_with_error(con);
                        }
                    }
                    if(strcmp(id,"4") == 0)
                    {
                        sprintf(query,"UPDATE Login SET Address='%s' WHERE Login_ID = '%s'",input,LoginID);
                        if (mysql_query(con, query))
                        {
                            printf("\nin query\n");
                            finish_with_error(con);
                        }
                    }
                    send(newsockfd,out,sizeof(out),0);
                }
                
                if(strcmp(option,"3") == 0)
                {
                    char query[300];
                    sprintf(query,"SELECT * FROM Bill WHERE Login_ID = '%s'",LoginID);
                    if (mysql_query(con, query))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    
                    MYSQL_RES *result = mysql_store_result(con);
                    if (result == NULL)
                    {
                        finish_with_error(con);
                    }
                    
                    int num_fields = mysql_num_fields(result);
                    MYSQL_ROW row;
                    char* send_buf[10];
                    char send_buffer[100];
                    bzero(send_buffer,sizeof(send_buffer));
                    while ((row = mysql_fetch_row(result)))
                    {
                        for(int i = 0; i < num_fields; i++)
                        {
                            send_buf[i] = row[i];
                            printf("%s\t",send_buf[i]);
                            strcat(send_buffer,send_buf[i]);
                            strcat(send_buffer,"\t");
                        }
                        send(newsockfd, send_buffer, sizeof(send_buffer), 0);
                        printf("\n");
                    }
                }
                if(strcmp(option,"4") == 0)
                {
                    char* send_buf[10];
                    char send_buffer3[500];
                    char setTopBoxID[5];
                    char confirm_purchase[5];
                    char set_top_box_name[20];
                    double amt;
                    char query[300];
                    bzero(setTopBoxID,sizeof(setTopBoxID));
                    bzero(send_buffer3,sizeof(send_buffer3));
                    sprintf(query,"SELECT * FROM Set_Top_Boxes");
                    if (mysql_query(con, query))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    
                    MYSQL_RES *result = mysql_store_result(con);
                    if (result == NULL)
                    {
                        finish_with_error(con);
                    }
                    int num_fields = mysql_num_fields(result);
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(result)))
                    {
                        for(int i = 0; i < num_fields; i++)
                        {
                            send_buf[i] = row[i];
                            printf("%s",send_buf[i]);
                            strcat(send_buffer3,send_buf[i]);
                            if(i != 1)
                            {
                                strcat(send_buffer3,"\t");
                            }
                            if(i == 1)
                            {
                                strcat(send_buffer3,"\t");
                            }
                            if(i == 2)
                            {
                                strcat(send_buffer3,"\t");
                            }
                        }
                        printf("\n");
                        strcat(send_buffer3,"\n");
                    }
                    send(newsockfd,send_buffer3,sizeof(send_buffer3),0);
                    recv(newsockfd,setTopBoxID,sizeof(setTopBoxID),0);
                    printf("%s",setTopBoxID);
                    fflush(stdout);
                    recv(newsockfd,confirm_purchase,sizeof(confirm_purchase),0);
                    if(strcmp(confirm_purchase,"Y") == 0)
                    {
                        char query[300];
                        sprintf(query,"SELECT SET_TOP_BOX_NAME,SET_TOP_BOX_AMOUNT FROM Set_Top_Boxes where SET_TOP_BOX_ID = '%s'",setTopBoxID);
                        if (mysql_query(con, query))
                        {
                            printf("\nin query\n");
                            finish_with_error(con);
                        }
                        
                        MYSQL_RES *result = mysql_store_result(con);
                        if (result == NULL)
                        {
                            finish_with_error(con);
                        }
                        int num_fields = mysql_num_fields(result);
                        MYSQL_ROW row;
                        int flag =0;
                        while ((row = mysql_fetch_row(result)))
                        {
                            for(int i = 0; i < num_fields; i++)
                            {
                                send_buf[i] = row[i];
                                printf("%s",send_buf[i]);
                                strcpy(set_top_box_name,send_buf[0]);
                                amt = atof(send_buf[1]);
                            }
                            flag = 1;
                            printf("\n");
                        }
                        if(flag == 1)
                        {
                            sprintf(query,"INSERT INTO Customer_SetTopBox_approve VALUES('%s','%s','%f')",LoginID,set_top_box_name,amt);
                            if (mysql_query(con, query))
                            {
                                printf("\nin query\n");
                                finish_with_error(con);
                            }
                            char msg[]="SUCCESS";
                            send(newsockfd,msg,sizeof(msg),0);
                            char email[100];
                            bzero(email,sizeof(email));
                            sprintf(query,"SELECT Email_ID FROM Login WHERE Login_ID = '%s'",LoginID);
                            if (mysql_query(con, query))
                            {
                                printf("\nin query\n");
                                finish_with_error(con);
                            }
                            result = mysql_store_result(con);
                            while ((row = mysql_fetch_row(result)))
                            {
                                strcpy(email,row[0]);
                                //printf("%s",email);
                                fflush(stdout);
                            }
                            char command[10000];
                            sprintf(command, "echo 'Your request for adding new set top box to the account has been accepted. Thank you.' | mail -s 'Confirmation Email' %s",email);
                            system(command);
                        }
                        else
                        {
                            char msg[]="FAILURE";
                            send(newsockfd,msg,sizeof(msg),0);
                        }
                    }
                }
                if(strcmp(option,"5") == 0)
                {
                    char* send_buf[10];
                    char send_buffer3[500];
                    char packageID[5];
                    char confirm_purchase[5];
                    char Package_Name[20];
                    double amt;
                    char query[300];
                    bzero(packageID,sizeof(packageID));
                    bzero(send_buffer3,sizeof(send_buffer3));
                    sprintf(query,"SELECT * FROM Packages");
                    if (mysql_query(con, query))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    
                    MYSQL_RES *result = mysql_store_result(con);
                    if (result == NULL)
                    {
                        finish_with_error(con);
                    }
                    int num_fields = mysql_num_fields(result);
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(result)))
                    {
                        for(int i = 0; i < num_fields; i++)
                        {
                            send_buf[i] = row[i];
                            printf("%s",send_buf[i]);
                            strcat(send_buffer3,send_buf[i]);
                            strcat(send_buffer3,"\t");
                            if(i == 2)
                            {
                                strcat(send_buffer3,"\t");
                            }
                        }
                        printf("\n");
                        strcat(send_buffer3,"\n");
                    }
                    send(newsockfd,send_buffer3,sizeof(send_buffer3),0);
                    recv(newsockfd,packageID,sizeof(packageID),0);
                    printf("%s",packageID);
                    fflush(stdout);
                    recv(newsockfd,confirm_purchase,sizeof(confirm_purchase),0);
                    if(strcmp(confirm_purchase,"Y") == 0)
                    {
                        char query[300];
                        sprintf(query,"SELECT Package_Name,Package_Amount FROM Packages where Package_Code = '%s'",packageID);
                        if (mysql_query(con, query))
                        {
                            printf("\nin query\n");
                            finish_with_error(con);
                        }
                        
                        MYSQL_RES *result = mysql_store_result(con);
                        if (result == NULL)
                        {
                            finish_with_error(con);
                        }
                        int num_fields = mysql_num_fields(result);
                        MYSQL_ROW row;
                        int flag =0;
                        while ((row = mysql_fetch_row(result)))
                        {
                            for(int i = 0; i < num_fields; i++)
                            {
                                send_buf[i] = row[i];
                                printf("%s",send_buf[i]);
                                strcpy(Package_Name,send_buf[0]);
                                amt = atof(send_buf[1]);
                            }
                            flag = 1;
                            printf("\n");
                        }
                        if(flag == 1)
                        {
                            sprintf(query,"INSERT INTO Customer_Packages VALUES('%s','%s','%f')",LoginID,Package_Name,amt);
                            if (mysql_query(con, query))
                            {
                                printf("\nin query\n");
                                finish_with_error(con);
                            }
                            char msg[]="SUCCESS";
                            send(newsockfd,msg,sizeof(msg),0);
                            char email[100];
                            bzero(email,sizeof(email));
                            sprintf(query,"SELECT Email_ID FROM Login WHERE Login_ID = '%s'",LoginID);
                            if (mysql_query(con, query))
                            {
                                printf("\nin query\n");
                                finish_with_error(con);
                            }
                            result = mysql_store_result(con);
                            while ((row = mysql_fetch_row(result)))
                            {
                                strcpy(email,row[0]);
                                //printf("%s",email);
                                fflush(stdout);
                            }
                            char command[10000];
                            sprintf(command, "echo 'Your request for adding new channel package to the account has been accepted. Thank you.' | mail -s 'Confirmation Email' %s",email);
                            system(command);
                        }
                        else
                        {
                            char msg[]="FAILURE";
                            send(newsockfd,msg,sizeof(msg),0);
                        }
                    }
                    
                }
                
                if(strcmp(option,"6") == 0)
                {
                    login_signup(newsockfd);
                }
                
            }
            if(strcmp(send_buf[0],"RETAILER") == 0)
            {
                if(strcmp(option,"1") == 0)
                {
                    char query2[300];
                    sprintf(query2,"SELECT * FROM Customer_SetTopBox_approve");
                    if (mysql_query(con, query2))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    
                    MYSQL_RES *result2 = mysql_store_result(con);
                    if (result2 == NULL)
                    {
                        finish_with_error(con);
                    }
                    
                    int num_fields2 = mysql_num_fields(result2);
                    MYSQL_ROW row2;
                    char* send_buf2[10];
                    char send_buffer2[1000];
                    bzero(send_buffer2,sizeof(send_buffer2));
                    printf("ID\tName\tAmount\n");
                    while ((row2 = mysql_fetch_row(result2)))
                    {
                        for(int i = 0; i < num_fields2; i++)
                        {
                            send_buf2[i] = row2[i];
                            printf("%s\t",send_buf2[i]);
                            fflush(stdout);
                            if(i != 0)
                            {
                                strcat(send_buffer2,"\t");
                            }
                            strcat(send_buffer2,send_buf2[i]);
                        }
                        strcat(send_buffer2,"\n");
                    }
                    fflush(stdout);
                    send(newsockfd, send_buffer2, sizeof(send_buffer2), 0);
                }
                
                if(strcmp(option,"2") == 0)
                {
                    char query2[300];
                    sprintf(query2,"SELECT * FROM Customer_SetTopBox_approve");
                    if (mysql_query(con, query2))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    
                    MYSQL_RES *result2 = mysql_store_result(con);
                    if (result2 == NULL)
                    {
                        finish_with_error(con);
                    }
                    
                    int num_fields2 = mysql_num_fields(result2);
                    MYSQL_ROW row2;
                    char* send_buf2[10];
                    char send_buffer2[1000];
                    bzero(send_buffer2,sizeof(send_buffer2));
                    printf("ID\tName\tAmount\n");
                    while ((row2 = mysql_fetch_row(result2)))
                    {
                        for(int i = 0; i < num_fields2; i++)
                        {
                            send_buf2[i] = row2[i];
                            printf("%s\t",send_buf2[i]);
                            fflush(stdout);
                            if(i != 0)
                            {
                                strcat(send_buffer2,"\t");
                            }
                            strcat(send_buffer2,send_buf2[i]);
                        }
                        strcat(send_buffer2,"\n");
                    }
                    fflush(stdout);
                    send(newsockfd, send_buffer2, sizeof(send_buffer2), 0);
                    char approval[5];
                    recv(newsockfd,approval,sizeof(approval),0);
                    if(strcmp(approval,"Y")==0)
                    {
                        char query[300];
                        sprintf(query,"INSERT INTO Customer_SetTopBox SELECT * FROM Customer_SetTopBox_approve");
                        if (mysql_query(con, query))
                        {
                            printf("\nin query\n");
                            finish_with_error(con);
                        }
                        
                        char query2[30000];
                        sprintf(query2,"DELETE FROM Customer_SetTopBox_approve");
                        if (mysql_query(con, query2))
                        {
                            printf("\nin query\n");
                            finish_with_error(con);
                        }
                    }
                }
                if(strcmp(option,"3") == 0)
                {
                    login_signup(newsockfd);
                }
            }
            if(strcmp(send_buf[0],"ADMIN") == 0)
            {
                if(strcmp(option,"1") == 0)
                {
                    char ID[100];
                    recv(newsockfd,ID,sizeof(ID),0);
                    char query[300];
                    sprintf(query,"SELECT * FROM Login WHERE Login_ID = '%s'",ID);
                    if (mysql_query(con, query))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    
                    MYSQL_RES *result = mysql_store_result(con);
                    if (result == NULL)
                    {
                        finish_with_error(con);
                    }
                    
                    int num_fields = mysql_num_fields(result);
                    MYSQL_ROW row;
                    char* send_buf[10];
                    char send_buffer[100];
                    printf("Login_ID\tEmail_ID\tPassword\tRole\t\tFull_Name\tPhone_Number\tAddress\n");
                    while ((row = mysql_fetch_row(result)))
                    {
                        for(int i = 0; i < num_fields; i++)
                        {
                            send_buf[i] = row[i];
                            printf("%s\t",send_buf[i]);
                            if(i != 0)
                            {
                                strcat(send_buffer,"\t");
                            }
                            if(i == 1)
                            {
                                strcat(send_buffer,"\t");
                            }
                            strcat(send_buffer,send_buf[i]);
                        }
                        send(newsockfd, send_buffer, sizeof(send_buffer), 0);
                        memset(&send_buffer[0], 0, sizeof(send_buffer));
                        printf("\n");
                    }
                }
                
                if(strcmp(option,"2") == 0)
                {
                    recv(newsockfd,signupdetails,sizeof(signupdetails),0);
                    
                    printf("\nThe signup details are - %s\n\n", signupdetails);
                    
                    char login[100], email[100], role[100], password[100], fname[100], pnumber[100], address[100];
                    char s[2] = " ";
                    
                    recv(newsockfd,fname,sizeof(fname),0);
                    
                    strcpy(login, strtok(signupdetails, s));
                    strcpy(email, strtok(NULL, s));
                    strcpy(password, strtok(NULL, s));
                    // strcpy(role, strtok(NULL, s));
                    strcpy(pnumber, strtok(NULL, s));
                    strcpy(role, "RETAILER");
                    
                    recv(newsockfd,address,sizeof(address),0);
                    printf("\nCredentials received from client are:\n\nLogin_ID : %s \nEmail_ID : %s \nPassword : %s \nRole : %s\n name: %s\n phone %s\n address %s",login,email,password,role,fname,pnumber,address);
                    char statement[1000];
                    sprintf(statement, "INSERT INTO Login VALUES('%s','%s','%s','%s','%s','%s','%s')", login,email,password,role,fname,pnumber,address);
                    if (mysql_query(con, statement))
                    {
                        finish_with_error(con);
                    }
                    char command[10000];
                    sprintf(command, "echo 'Your retailer account has been setup. ' | mail -s 'Confirmation Email' %s", email);
                    system(command);
                }
                
                if(strcmp(option,"3") == 0)
                {
                    char ID[100];
                    char email[100];
                    recv(newsockfd,ID,sizeof(ID),0);
                    
                    
                    char query[300];
                    sprintf(query,"SELECT Email_ID FROM Login WHERE Login_ID = '%s'",ID);
                    if (mysql_query(con, query))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    MYSQL_RES *result = mysql_store_result(con);
                    if (result == NULL)
                    {
                        finish_with_error(con);
                    }
                    int num_fields = mysql_num_fields(result);
                    char* send_buf[100];
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(result)))
                    {
                        for(int i = 0; i < num_fields; i++)
                        {
                            send_buf[i] = row[i];
                            strcpy(email,send_buf[i]);
                        }
                    }
                    printf("Email ID fetched from the database is %s", email);
                    
                    
                    char query1[300];
                    sprintf(query1,"DELETE FROM Login WHERE Login_ID = '%s'",ID);
                    if (mysql_query(con, query1))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    send(newsockfd,email,sizeof(email),0);
                }
                
                if(strcmp(option,"4") == 0)
                {
                    char ID[100];
                    char email[100];
                    recv(newsockfd,ID,sizeof(ID),0);
                    
                    
                    char query[300];
                    sprintf(query,"SELECT Email_ID FROM Login WHERE Login_ID = '%s'",ID);
                    if (mysql_query(con, query))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    MYSQL_RES *result = mysql_store_result(con);
                    if (result == NULL)
                    {
                        finish_with_error(con);
                    }
                    int num_fields = mysql_num_fields(result);
                    char* send_buf[100];
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(result)))
                    {
                        for(int i = 0; i < num_fields; i++)
                        {
                            send_buf[i] = row[i];
                            strcpy(email,send_buf[i]);
                        }
                    }
                    printf("Email ID fetched from the database is %s", email);
                    
                    
                    char query1[300];
                    sprintf(query1,"DELETE FROM Login WHERE Login_ID = '%s'",ID);
                    if (mysql_query(con, query1))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    send(newsockfd,email,sizeof(email),0);
                }
                if(strcmp(option,"5") == 0)
                {
                    char packageID[100];
                    char packageName[100];
                    char packageDesc[100];
                    char packageAmount[100];
                    recv(newsockfd, packageID, sizeof(packageID), 0);
                    recv(newsockfd, packageName, sizeof(packageName), 0);
                    recv(newsockfd, packageDesc, sizeof(packageDesc), 0);
                    recv(newsockfd, packageAmount, sizeof(packageAmount), 0);
                    double amt = atof(packageAmount);
                    char statement[1000];
                    sprintf(statement, "INSERT INTO Packages VALUES('%s','%s','%s','%f')", packageID,packageName,packageDesc,amt);
                    if (mysql_query(con, statement))
                    {
                        finish_with_error(con);
                    }
                }
                if(strcmp(option,"6") == 0)
                {
                    char packageID[100];
                    recv(newsockfd, packageID, sizeof(packageID), 0);
                    char statement[1000];
                    sprintf(statement, "DELETE FROM Packages WHERE Package_code = '%s'", packageID);
                    if (mysql_query(con, statement))
                    {
                        finish_with_error(con);
                    }
                }
                if(strcmp(option,"7") == 0)
                {
                    char customerID[100];
                    bzero(customerID,sizeof(customerID));
                    recv(newsockfd, customerID, sizeof(customerID), 0);
                    //printf("cust id -%s",customerID);
                    fflush(stdout);
                    char statement[1000];
                    sprintf(statement, "SELECT Package_Amount FROM Customer_Packages WHERE Login_ID = '%s'", customerID);
                    if (mysql_query(con, statement))
                    {
                        finish_with_error(con);
                    }
                    MYSQL_RES *result = mysql_store_result(con);
                    if (result == NULL)
                    {
                        finish_with_error(con);
                    }
                    int num_fields = mysql_num_fields(result);
                    char* send_buf[100];
                    MYSQL_ROW row;
                    double total_amount =0;
                    while ((row = mysql_fetch_row(result)))
                    {
                        printf("%s\t",row[0]);
                        total_amount += atof(row[0]);
                    }
                    printf("Total: $%f",total_amount);
                    fflush(stdout);
                    char date[]="12/31/2017";
                    sprintf(statement, "INSERT INTO Bill VALUES('%s','%f','%s')", customerID,total_amount,date);
                    if (mysql_query(con, statement))
                    {
                        finish_with_error(con);
                    }
                    
                    char query[300];
                    char email[100];
                    bzero(email,sizeof(email));
                    sprintf(query,"SELECT Email_ID FROM Login WHERE Login_ID = '%s'",customerID);
                    if (mysql_query(con, query))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    result = mysql_store_result(con);
                    while ((row = mysql_fetch_row(result)))
                    {
                        strcpy(email,row[0]);
                        //printf("%s",email);
                        fflush(stdout);
                    }
                    char command[10000];
                    sprintf(command, "echo 'Your bill has been generated. The total amount is $'%f' and the payment due date is '%s'.' | mail -s 'Bill Generation Email' %s", total_amount, date,email);
                    system(command);
                }
                if(strcmp(option,"8") == 0)
                {
                    char customerID[100];
                    bzero(customerID,sizeof(customerID));
                    recv(newsockfd, customerID, sizeof(customerID), 0);
                    //printf("cust id -%s",customerID);
                    fflush(stdout);
                    char statement[1000];
                    sprintf(statement, "DELETE FROM Bill WHERE Login_ID = '%s'", customerID);
                    if (mysql_query(con, statement))
                    {
                        finish_with_error(con);
                    }
                    char query[300];
                    char email[100];
                    bzero(email,sizeof(email));
                    sprintf(query,"SELECT Email_ID FROM Login WHERE Login_ID = '%s'",customerID);
                    if (mysql_query(con, query))
                    {
                        printf("\nin query\n");
                        finish_with_error(con);
                    }
                    result = mysql_store_result(con);
                    while ((row = mysql_fetch_row(result)))
                    {
                        strcpy(email,row[0]);
                        //printf("%s",email);
                        fflush(stdout);
                    }
                    char command[10000];
                    sprintf(command, "echo 'Thank You for the payment.' | mail -s 'Payment Confirmation Email' %s",email);
                    system(command);
                }
                if(strcmp(option,"9") == 0)
                {
                    login_signup(newsockfd);
                }
            }
        }
        mysql_free_result(result);
    }
    else if(flag == 2)
    {
        recv(newsockfd,signupdetails,sizeof(signupdetails),0);
        printf("\nThe signup details are - %s\n\n", signupdetails);
        char login[100], email[100], role[100], password[100], fname[100], pnumber[100], address[100];
        char s[2] = " ";
        recv(newsockfd,fname,sizeof(fname),0);
        strcpy(email, strtok(signupdetails, s));
        strcpy(password, strtok(NULL, s));
        strcpy(pnumber, strtok(NULL, s));
        
        strcpy(role,"CUSTOMER");
        recv(newsockfd,address,sizeof(address),0);
        
        strcpy(login,"C");
        char initial[100];
        int x;
        srand (time(NULL));
        x = (rand()%2999)+7000;
        //initial = itoa(x);
        sprintf(initial, "%d", x);
        strcat(login,initial);
        printf("\nCredentials received from client are:\n\nLogin_ID : %s \nEmail_ID : %s \nPassword : %s \nRole : %s\nName : %s\nPhone :  %s\nAddress :  %s",login,email,password,role,fname,pnumber,address);
        char statement[1000];
        sprintf(statement, "INSERT INTO Login VALUES('%s','%s','%s','%s','%s','%s','%s')", login,email,password,role,fname,pnumber,address);
        if (mysql_query(con, statement))
        {
            finish_with_error(con);
        }
        char command[10000];
        
        sprintf(command, "echo 'Your request for new Account has been received. Please note the Login ID %s for future correspondence. A Admin will review the profile information and you will receive another email regarding your account update. ' | mail -s 'Confirmation Email' %s", login, email);
        system(command);
        send(newsockfd,email,sizeof(email),0);
        login_signup(newsockfd);
    }
    mysql_close(con);
    return 0;
}
