#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SERVER_PORT 9876
#define BUFFER_SIZE 1024


// funcao que verifica se existe ficheiro e enviar mensagem para o cliente
bool file_exists(const char *filename,int client_sock, int f) {
    FILE *file_ptr = fopen(filename, "r");
    if (file_ptr != NULL) {
   // se existir um ficheiro e estiver a fazer login, envia uma mensagem a pedir a password
if (send(client_sock, "\nInsert your password: ", strlen("\nInsert your password: "), 0) == -1) {
            perror("Error sending menu to client");
            exit(EXIT_FAILURE);
        }
        fclose(file_ptr);
        return true;
    }

if(file_ptr == NULL && f==1){if (send(client_sock, "\nUsername accepted!\n", strlen("\nUsername accepted!\n"), 0) == -1) {
            perror("Error sending menu to client");
            exit(EXIT_FAILURE);
        }

return false;}
// se nao existir ficheiro e estiver a fazer login, envia mensagem a informar o cliente que o utilizador nao foi encontrado
if (send(client_sock, "\nUsername not found\n\n", strlen("\nUsername not found\n\n"), 0) == -1) {
            perror("Error sending menu to client");
            exit(EXIT_FAILURE);
        }
    return false;


}

//funcao para adicionar uma password ao ficheiro
void add_password(const char *filename, const char *password) {
    FILE *file_ptr = fopen(filename, "a");
    if (file_ptr == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }

//escreve a pass no ficheiro
    fprintf(file_ptr, "%s\n", password);
    fclose(file_ptr);
}

//funcao que verifica de a senha esta correta
bool search_password(const char *filename, const char *password, int client_sock) {
    FILE *file_ptr = fopen(filename, "r");
    if (file_ptr == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char buffer[100]; // Assuming maximum password length is 100 characters

    while (fgets(buffer, sizeof(buffer), file_ptr) != NULL) {
        // Remove o caactere de nova linha, se existir
        buffer[strcspn(buffer, "\n")] = '\0';

        // Compara a passsword lida com a do ficheiro
        if (strcmp(buffer, password) == 0) {
if (send(client_sock, "\nLogin successful!\n\n", strlen("\nLogin successful!\n\n"), 0) == -1) {
            perror("Error sending menu to client");
            exit(EXIT_FAILURE);
        }
            fclose(file_ptr);
            return true; // Password found
        }
    }
if (send(client_sock, "\n\nPassword or username are incorrect. Please try again\n\n", strlen("\n\nPassword or username are incorrect. Please try again\n\n"), 0) == -1) {
            perror("Error sending menu to client");
            exit(EXIT_FAILURE);
        }
    fclose(file_ptr);
    return false; // Password not found
}

//funcao para criar uma nova conta de utilzador
bool sign_up(const char *filename,int client_sock) {
    FILE *file_ptr;

//verifica so o ficheiro para o utilizador ja existe
    if (file_exists(filename,client_sock,1)) {
    //se existir envia mensagem de erro
        printf("File '%s' already exists.\n", filename);
if (send(client_sock, "\n\nThis username is alerady in use. Please try again\n\n", strlen("\n\nThis username is alerady in use. Please try again\n\n"), 0) == -1) {
            perror("Error sending menu to client");
            exit(EXIT_FAILURE);
        }
        return false;
    }

    // Attempt to open the file for writing
    file_ptr = fopen(filename, "w");

    if (file_ptr == NULL) {
        fprintf(stderr, "Error creating file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Close the file
    fclose(file_ptr);

    printf("File '%s' created successfully.\n", filename);
if (send(client_sock, "\nInsert your password: ", strlen("\nInsert your password: "), 0) == -1) {
            perror("Error sending menu to client");
            exit(EXIT_FAILURE);
        }
	return true;
}

//funcao para enviar o menu para o cliente
void send_menu(int client_sock) {
    char menu[] = "User Menu:\n1. Sign-up\n2. Log-in\n0. Logout\nEnter your choice: ";
    if (send(client_sock, menu, strlen(menu), 0) == -1) {
        perror("Error sending menu to client");
        exit(EXIT_FAILURE);
    }

}

//funcao para gerir as opcoes do menu
int menu(int client_sock, char *choice) {
    char response[BUFFER_SIZE];

    if (choice[0] == '1') {
        if (send(client_sock, "\nInsert your username: ", strlen("\nInsert your username: "), 0) == -1) {
            perror("Error sending menu to client");
            exit(EXIT_FAILURE);
        }
        return 1;
    } else if (choice[0] == '2') {
        if (send(client_sock, "\nInsert your username: ", strlen("\nInsert your username: "), 0) == -1) {
            perror("Error sending menu to client");
            exit(EXIT_FAILURE);
        }
        return 2;
    } else if (choice[0] == '0') {
        if (send(client_sock, "\nYou have logged out", strlen("\nYou have logged out"), 0) == -1) {
            perror("Error sending menu to client");
            exit(EXIT_FAILURE);
        }
        return 0;
    } else {
        snprintf(response, sizeof(response), "\n\nInvalid choice. Please try again.\n\n");
    }

    if (send(client_sock, response, strlen(response), 0) == -1) {
        perror("Error sending response to client");
        exit(EXIT_FAILURE);
    }

    return -1; // Indicate invalid choice
}


int main() {
    int sockfd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    char username[BUFFER_SIZE];
    char choice;

    // Create TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sockfd, 5) == -1) {
        perror("Error listening for connections");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    // Accept connections from clients
    socklen_t client_addr_len = sizeof(client_addr);
    if ((client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
        perror("Error accepting connection");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");

   

  memset(buffer, 0, BUFFER_SIZE);    

    //loop pricipal para receber as escolhas do cliente
    while (1) {    
  // envia o menu para o cliente
    send_menu(client_sock);
        // Recebe a escoha do cliente
        ssize_t recv_len = recv(client_sock, buffer, BUFFER_SIZE, 0);


menu(client_sock,buffer);        
if (recv_len <= 0) {
            perror("Error receiving choice from client");
            break;
        }

        // Null termina a escolha recebida
        buffer[recv_len] = '\0';
choice= buffer[0];
        // Print client's choice
        printf("Client choice: %s\n", buffer);

if(choice=='1'){
memset(buffer, 0, BUFFER_SIZE);
recv_len = recv(client_sock, buffer, BUFFER_SIZE, 0);
strcpy(username, buffer);
if(sign_up(buffer, client_sock)==true){
memset(buffer, 0, BUFFER_SIZE);
recv_len = recv(client_sock, buffer, BUFFER_SIZE, 0);
add_password(username,buffer);}
}
        if(choice=='2'){
memset(buffer, 0, BUFFER_SIZE);
recv_len = recv(client_sock, buffer, BUFFER_SIZE, 0);
strcpy(username, buffer);
if(file_exists(buffer, client_sock,2)){
memset(buffer, 0, BUFFER_SIZE);
recv_len = recv(client_sock, buffer, BUFFER_SIZE, 0);
search_password(username,buffer,client_sock);}

}
 

  memset(buffer, 0, BUFFER_SIZE);    

}

    // Close sockets
    close(client_sock);
    close(sockfd);

    return 0;
}

