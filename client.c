#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void print_stars()
{
    printf("\n****************************************************************************************************************************\n");
}

void print_lines()
{
    printf("-----------------------------\n");
    
}

int login_signup(int sockfd);

void customer_module(char *LoginID, char *Password, int sockfd)
{
    char option[5];
    
    printf("\n\n\t\t\t\t\t*****  Customer logged in as '%s'  *****\n\n",LoginID);
    printf("\n");
    print_lines();
    printf("Please select one option:\n");
    printf("1. View Profile\n");
    printf("2. Edit Profile\n");
    printf("3. View Account Bill\n");
    printf("4. Purchase a set top box\n");
    printf("5. Purchase a package\n");
    printf("6. Exit\n");
    print_lines();
    printf("\nEnter your choice:\t");
    scanf("%s", option);
    send(sockfd, option, sizeof(option), 0);
    if(strcmp(option,"1") == 0)
    {
        system("clear");
        char recv_buffer[100];
        bzero(recv_buffer,sizeof(recv_buffer));
        recv(sockfd,recv_buffer,sizeof(recv_buffer),0);
        printf("\nProfile Details are -\n");
        print_stars();
        printf("Login_ID\tEmail_ID\t\tPassword\tRole\t\tFull_Name\tPhone_Number\tAddress\n");
        printf("%s",recv_buffer);
        print_stars();
        memset(&recv_buffer, 0, sizeof(recv_buffer));
        //char recv_buffer[100] = {0};
        //bzero(recv_buffer,100);
    }
    if(strcmp(option,"2") == 0)
    {
        system("clear");
        int edit;
        print_lines();
        printf("Please select one to edit:\n");
        printf("1. Email ID\n");
        printf("2. Password\n");
        printf("3. Phone Number\n");
        printf("4. Address\n");
        printf("5. Done\n");
        print_lines();
        printf("\nEnter your choice:\t");
        scanf("%d",&edit);
        switch(edit)
        {
            char out[30];
            case 1:
            {
                char emailid[20];
                char id[5]="1";
                send(sockfd,id,sizeof(id),0);
                printf("\nPlease enter the new Email Address:\t");
                scanf("%s",emailid);
                send(sockfd,emailid,sizeof(emailid),0);
                recv(sockfd,out,sizeof(out),0);
                print_stars();
                printf("\t\t\t\t\t%s",out);
                print_stars();
                customer_module(LoginID,Password,sockfd);
            }
            case 2:
            {
                char pass[20];
                char id[5]="2";
                send(sockfd,id,sizeof(id),0);
                printf("\nPlease enter the new Password:\t");
                scanf("%s",pass);
                send(sockfd,pass,sizeof(pass),0);
                recv(sockfd,out,sizeof(out),0);
                print_stars();
                printf("\t\t\t\t\t%s",out);
                print_stars();
                customer_module(LoginID,Password,sockfd);
            }
            case 3:
            {
                char number[20];
                char id[5]="3";
                send(sockfd,id,sizeof(id),0);
                printf("\nPlease enter the new Phone Number:\t");
                scanf("%s",number);
                send(sockfd,number,sizeof(number),0);
                recv(sockfd,out,sizeof(out),0);
                print_stars();
                printf("\t\t\t\t\t%s",out);
                print_stars();
                customer_module(LoginID,Password,sockfd);
            }
            case 4:
            {
                char address[20];
                char id[5]="4";
                send(sockfd,id,sizeof(id),0);
                printf("\nPlease enter the new Address:\t");
                scanf(" %[^\n]",address);
                send(sockfd,address,sizeof(address),0);
                recv(sockfd,out,sizeof(out),0);
                print_stars();
                printf("\t\t\t\t\t%s",out);
                print_stars();
                customer_module(LoginID,Password,sockfd);
            }
            case 5:
            {
                customer_module(LoginID,Password,sockfd);
            }
        }
    }
    if(strcmp(option,"3") == 0)
    {
        system("clear");
        char balance[100];
        bzero(balance,sizeof(balance));
        recv(sockfd,balance,sizeof(balance),0);
        print_stars();
        printf("Account Bill Details -\n");
        printf("ID\tAmount\tDue Date\n");
        printf("%s",balance);
        print_stars();
    }
    if(strcmp(option,"4") == 0)
    {
            system("clear");
            char setTopBoxID[5];
            char confirm_purchase[5];
            char message[500];
            char msg[20];
            bzero(message,sizeof(message));
            bzero(setTopBoxID,sizeof(setTopBoxID));
            recv(sockfd,message,sizeof(message),0);
            print_stars();
            printf("The available Set Top Boxes are-\n");
            printf("\nID\tName\t\tDescription\t\t\t\t\t\t\t\t\tAmount in $\n");
            printf("%s",message);
            print_stars();
            printf("\nPlease select the Set Top Box-\t");
            scanf(" %[^\n]",setTopBoxID);
            send(sockfd,setTopBoxID,sizeof(setTopBoxID),0);
            fflush(stdout);
            printf("Do you want to confirm your purchase for set top box '%s' (Y/N)\t",setTopBoxID);
            scanf("%s",confirm_purchase);
            if(strcmp(confirm_purchase,"Y")==0)
            {
                send(sockfd,confirm_purchase,sizeof(confirm_purchase),0);
                recv(sockfd,msg,sizeof(msg),0);
                if(strcmp(msg,"SUCCESS")==0)
                {
                    print_stars();
                    printf("\t\t\t\tSet top box added successfully");
                    print_stars();
                }
                else
                {
                    print_stars();
                    printf("\t\t\t\tUnable to add set top box");
                    print_stars();
                }
            }
            else
            {
                customer_module(LoginID,Password,sockfd);
            }
    }
    if(strcmp(option,"5") == 0)
    {
        system("clear");
        char packageID[5];
        char confirm_purchase[5];
        char message[500];
        char msg[20];
        bzero(message,sizeof(message));
        bzero(packageID,sizeof(packageID));
        recv(sockfd,message,sizeof(message),0);
        print_stars();
        printf("The available packages are-\n");
        printf("\nID\tName\t\tDescription\t\tAmount in $\n");
        printf("%s",message);
        print_stars();
        printf("\nPlease select the package-\t");
        scanf(" %[^\n]",packageID);
        send(sockfd,packageID,sizeof(packageID),0);
        fflush(stdout);
        printf("Do you want to confirm your purchase for package '%s' (Y/N)\t",packageID);
        scanf("%s",confirm_purchase);
        if(strcmp(confirm_purchase,"Y")==0)
        {
            send(sockfd,confirm_purchase,sizeof(confirm_purchase),0);
            recv(sockfd,msg,sizeof(msg),0);
            if(strcmp(msg,"SUCCESS")==0)
            {
                print_stars();
                printf("\t\t\t\tPackage added successfully");
                print_stars();
            }
            else
            {
                print_stars();
                printf("\t\t\t\tUnable to add package");
                print_stars();
            }
        }
        else
        {
            customer_module(LoginID,Password,sockfd);
        }
    }
    if(strcmp(option,"6") ==0)
    {
        system("clear");
        login_signup(sockfd);
    }
    customer_module(LoginID,Password,sockfd);
}

void retailer_module(char *LoginID, char *Password, int sockfd)
{
    char option[5];
    printf("\n\n\t\t\t\t\t*****  Retailer logged in as '%s'  *****\n\n",LoginID);
    printf("\n");
    print_lines();
    printf("Please select one option:\n");
    printf("1. View pending requests\n");
    printf("2. Approve pending requests\n");
    printf("3. Exit\n");
    print_lines();
    printf("\nEnter your choice:\t");
    scanf("%s", option);
    send(sockfd, option, sizeof(option), 0);
    if(strcmp(option,"1") == 0)
    {
        system("clear");
        char recv_buffer[1000];
        bzero(recv_buffer,sizeof(recv_buffer));
        printf("\nPending customer requests are - \n\n");
        recv(sockfd,recv_buffer,sizeof(recv_buffer),0);
        print_stars();
        printf("ID\tName\t\tAmount in $\n");
        printf("%s",recv_buffer);
        print_stars();
    }
    if(strcmp(option,"2") == 0)
    {
        system("clear");
        char recv_buffer[1000];
        bzero(recv_buffer,sizeof(recv_buffer));
        printf("\nPending customer requests are - \n\n");
        recv(sockfd,recv_buffer,sizeof(recv_buffer),0);
        print_stars();
        printf("ID\tName\t\tAmount in $\n");
        printf("%s",recv_buffer);
        print_stars();
        //memset(&recv_buffer, 0, sizeof(recv_buffer));
        printf("Do you want to approve? (Y/N):\t");
        char approval[5];
        scanf("%s",approval);
        send(sockfd,approval,sizeof(approval),0);
        if(strcmp(approval,"Y") == 0)
        {
            print_stars();
            printf("\t\t\tThe above pending set top box requests have been approved");
            print_stars();
        }  
    }
    if(strcmp(option,"3") ==0)
    {
        system("clear");
        login_signup(sockfd);
    }
    retailer_module(LoginID,Password,sockfd);
}


void admin_module(char *LoginID, char *Password, int sockfd)
{
    char option[5];
    char recv_buffer[100];
    printf("\n\n\t\t\t\t\t*****  Admin logged in as '%s'  *****\n\n",LoginID);
    printf("\n");
    print_lines();
    printf("Please select one option:\n");
    printf("1. View Customer Profile\n");
    printf("2. Setup new retailer account\n");
    printf("3. Delete customer\n");
    printf("4. Delete retailer\n");
    printf("5. Add packages\n");
    printf("6. Delete packages\n");
    printf("7. Generate bill for customer\n");
    printf("8. Update customer payment\n");
    printf("9. Exit\n");
    print_lines();
    printf("\nEnter your choice:\t");
    scanf("%s", option);
    send(sockfd, option, sizeof(option), 0);
    
    if(strcmp(option,"1") == 0)
    {
        system("clear");
        char ID[100];
        
        printf("Enter the customer ID:\t");
        scanf("%s", ID);
        send(sockfd,ID,sizeof(ID),0);
        recv(sockfd,recv_buffer,sizeof(recv_buffer),0);
        print_stars();
        printf("Login_ID\tEmail_ID\t\tPassword\tRole\t\tFull_Name\tPhone_Number\tAddress\n");
        printf("%s",recv_buffer);
        print_stars();
        memset(&recv_buffer, 0, sizeof(recv_buffer));
    }
    
    
    if(strcmp(option,"2") == 0)
    {
        system("clear");
        printf("\nEnter the following details for creating a new account\t");
        
        printf("\nEnter Email ID:\t");
        char email[100];
        scanf("%s",email);
        
        printf("Enter password:\t");
        char pass[100];
        scanf("%s",pass);
        
        printf("Enter Login ID:\t");
        char lg[100];
        scanf("%s",lg);
        
        printf("Enter your full name:\t");
        char fname[100];
        scanf(" %[^\n]",fname);
        
        printf("Enter your Phone Number:\t");
        char pnumber[100];
        scanf("%s", pnumber);
        
        printf("Enter your address:\t");
        char address[100];
        scanf(" %[^\n]",address);
        
        char signupdetails[500];
        strcpy(signupdetails,"");
        strcat(signupdetails,lg);
        strcat(signupdetails," ");
        strcat(signupdetails,email);
        strcat(signupdetails," ");
        strcat(signupdetails,pass);
        strcat(signupdetails," ");
        strcat(signupdetails,pnumber);
        strcat(signupdetails," ");
        
        send(sockfd, signupdetails, sizeof(signupdetails), 0);
        send(sockfd, fname, sizeof(fname), 0);
        send(sockfd, address, sizeof(address), 0);
        print_stars();
        printf("\t\t\t\tRetailer Account created");
        print_stars();
    }
    
    if(strcmp(option,"3") == 0)
    {
        system("clear");
        char ID[100];
        char email[100];
        printf("Enter the Customer ID:\t");
        scanf("%s",ID);
        send(sockfd,ID,sizeof(ID),0);
        recv(sockfd,email,sizeof(email),0);
        print_stars();
        printf("\t\tThe customer with login ID '%s' and email ID '%s' has been deleted",ID,email);
        print_stars();
    }
    
    if(strcmp(option,"4") == 0)
    {
        system("clear");
        char ID[100];
        char email[100];
        printf("Enter the Retailer ID:\t");
        scanf("%s",ID);
        send(sockfd,ID,sizeof(ID),0);
        recv(sockfd,email,sizeof(email),0);
        print_stars();
        printf("\t\tThe retailer with login ID '%s' and email ID '%s' has been deleted",ID,email);
        print_stars();
    }
    if(strcmp(option,"5") == 0)
    {
        system("clear");
        char packageID[100];
        char packageName[100];
        char packageDesc[100];
        char packageAmount[100];
        
        printf("Enter the package ID:\t");
        scanf("%s",packageID);
        printf("Enter the package name:\t");
        scanf(" %[^\n]",packageName);
        printf("Enter the package description:\t");
        scanf(" %[^\n]", packageDesc);
        printf("Enter the package amount:\t");
        scanf(" %[^\n]",packageAmount);
        send(sockfd, packageID, sizeof(packageID), 0);
        send(sockfd, packageName, sizeof(packageName), 0);
        send(sockfd, packageDesc, sizeof(packageDesc), 0);
        send(sockfd, packageAmount, sizeof(packageAmount), 0);
        print_stars();
        printf("\t\t\t\t\tPackage '%s' added successfully",packageID);
        print_stars();
    }
    if(strcmp(option,"6") == 0)
    {
        system("clear");
        char packageID[100];
        bzero(packageID,sizeof(packageID));
        printf("Enter the package ID:\t");
        scanf("%s",packageID);
        send(sockfd, packageID, sizeof(packageID), 0);
        print_stars();
        printf("\t\t\t\t\tPackage '%s' deleted successfully",packageID);
        print_stars();
    }
    if(strcmp(option,"7") == 0)
    {
        system("clear");
        char customerID[100];
        bzero(customerID,sizeof(customerID));
        printf("Enter the customer ID:\t");
        scanf("%s",customerID);
        //printf("cust id -%s",customerID);
        send(sockfd, customerID, sizeof(customerID), 0);
        print_stars();
        printf("\t\t\t\t\tBill for customer '%s' generated successfully",customerID);
        print_stars();
    }
    if(strcmp(option,"8") == 0)
    {
        system("clear");
        char customerID[100];
        bzero(customerID,sizeof(customerID));
        printf("Enter the customer ID:\t");
        scanf("%s",customerID);
        //printf("cust id -%s",customerID);
        send(sockfd, customerID, sizeof(customerID), 0);
        print_stars();
        printf("\t\t\t\t\tBill for customer '%s' updated successfully",customerID);
        print_stars();
    }
    
    if(strcmp(option,"9") ==0)
    {
        system("clear");
        login_signup(sockfd);
    }
    admin_module(LoginID,Password,sockfd);
}


int login_signup(int sockfd)
{
    ssize_t recv_bytes;
    char recv_buff[100];
    char LoginID[20];
    char Password[20];
    char LoginDetails[20];
    system("clear");
    printf("\n\n_________________________________________________________________________");
    printf("_____________________________________________________\n");
    printf("\n\t\t\t\t\t\tDTH Services management System\n");
    printf("_________________________________________________________________________");
    printf("_____________________________________________________\n");
    printf("\nEnter 1 to log in existing account (LogIn)");
    printf("\nEnter 2 to create a new account (SignUp)\n");
    printf("\nEnter your choice:\t");
    int flag = 0;
    scanf("%d", &flag);
    
    if(flag == 1)
    {
        system("clear");
        send(sockfd, &flag, sizeof(int), 0);
        
        printf("\nPlease enter your username\t");
        scanf("%s", LoginID);
        printf("Please enter your Password\t");
        scanf("%s", Password);
        
        strcpy(LoginDetails,"");
        strcat(LoginDetails,LoginID);
        strcat(LoginDetails," ");
        strcat(LoginDetails,Password);
        
        send(sockfd, LoginDetails, sizeof(LoginDetails), 0);
        
        if((recv_bytes=recv(sockfd,recv_buff,sizeof(recv_buff),0))>0)
        {
            if(strcmp(recv_buff,"CUSTOMER") ==0)
            {
                customer_module(LoginID,Password,sockfd);
            }
            else if(strcmp(recv_buff,"RETAILER") ==0)
            {
                retailer_module(LoginID,Password,sockfd);
            }
            else if(strcmp(recv_buff,"ADMIN") ==0)
            {
                admin_module(LoginID,Password,sockfd);
            }
            else if(strcmp(recv_buff,"INVALID") == 0)
            {

                print_stars();
                printf("\t\t\t\t\tThe LoginID and Password is invalid");
                print_stars();
                login_signup(sockfd);
            }
        }
    }
    else if(flag == 2)
    {
        system("clear");
        send(sockfd, &flag, sizeof(int), 0);
        
        printf("\nEnter the following details for creating a new account\t");
        
        printf("\nEnter Email ID:\t");
        char email[100];
        scanf("%s",email);
        
        printf("Enter password:\t");
        char pass[100];
        scanf("%s",pass);
        
        printf("Enter your full name:\t");
        char fname[100];
        scanf(" %[^\n]",fname);
        
        printf("Enter your Phone Number:\t");
        char pnumber[100];
        scanf("%s", pnumber);
        
        printf("Enter your address:\t");
        char address[100];
        scanf(" %[^\n]",address);
        
        char signupdetails[500];
        strcpy(signupdetails,"");
        strcat(signupdetails,email);
        strcat(signupdetails," ");
        strcat(signupdetails,pass);
        strcat(signupdetails," ");
        strcat(signupdetails,pnumber);
        strcat(signupdetails," ");
        send(sockfd, signupdetails, sizeof(signupdetails), 0);
        send(sockfd, fname, sizeof(fname), 0);
        send(sockfd, address, sizeof(address), 0);
        
        char test[100];
        recv(sockfd,test,sizeof(test),0);
        print_stars();
        printf("Thank you for choosing the TCP/IP Bank.");
        printf("Email containing the Login credentials have been sent to '%s'.\n", test);
        printf("Your profile is under review. Once approved, you will receive an email regarding the same.\n");
        print_stars();
        login_signup(sockfd);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    /* 
    *  sockfd is the socket descriptor which is an integer value
    *  filename is used to store the name of the file 
    */
    int sockfd;
    struct sockaddr_in serv_addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    /* 
    *  Socket call will return a socket descriptor on success which is an integer 
    *  And it will return '-1' for error
    */
    if (sockfd == -1) 
    {
        printf("\nerror calling socket\n");
        exit(1);
    }

    /* Populating the sockaddr_in struct with the following values */
    /* Assigning the AF_INET (Internet Protocol v4) address family */
    serv_addr.sin_family = AF_INET; 

    /* Populating the Server IP address with the value of the localhost IP address */
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Converting the port number received from host byte order to network byte order */
    serv_addr.sin_port = htons(8000);

    /* 
    *  Connect () takes three arguments:
    *  The sockect descriptor
    *  Server Address Structure
    *  And the size of Address 
    */
    if (connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
    /* Connect() returns '-1' on failure and 0 on success */
    {
        printf("\nError connecting to server\n");
        exit(1);
    }
    
    login_signup(sockfd);
    close(sockfd);
    return 0;
}
