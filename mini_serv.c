/**
 * DESCRIPTION:
 * one-file solution to meet the projects goals
 * only the pre-given helper functions are included by an external file to not have to copy them into this file
 * this file can be used to solve the final exam at 42 (2026 version) by removing the debug commands 
 * and the corresponding header as well as the signal handling and copy the functions of the file mini_serv_helpers.c
 */

#include "debug.h"
#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 65536

//from mini_serv_helpers.c
int extract_message(char **buf, char **msg);
char *str_join(char *buf, char *add);

// global variables
fd_set	fds_read;
fd_set	fds_write;
int	max_fd = 0;
int	last_id = 0;
int	ids[MAX_CLIENTS];
char	*outbuf[MAX_CLIENTS];


// optional global variable to exit clean
volatile sig_atomic_t g_signalnum = 0;

/**
 * @brief signal handler function that sets the global variable g_signalnum to the received signal number
 * this variable is used in the main server loop to break the loop and exit the program in a clean way when a signal is received
 * the signal handler is set in main.c for the signals SIGINT, SIGTERM, SIGQUIT and SIGPIPE
 * (SIGPIPE is ignored since it can occur when trying to write to a closed socket)
 *
 * @param sig the signal number received
 */
void signal_handler(int sig)
{
	(void)sig;
	DEBUG_PRINT("Debug: signal handler called\n");
	g_signalnum = sig;
}

/**
 * @brief helper function provide a clean exit in case of an error
 * (1) it returns an error msg
 * (2) it closes the fds of all connected clients
 * (3) it closes the socket fd of the server itself
 */
void ft_err_exit(int socket_fd)
{
	char	err_msg[] = "Fatal error\n";
	write(2, err_msg, strlen(err_msg));
	// Close fds
	for (int i = 0; i <= max_fd; i++) {
		if (ids[i])
			close (i);
	}
	if (socket_fd >= 0)
		close(socket_fd);
	exit(1);
}

void ft_send_msg(int sockfd, int fd, char *msg) { 
	for (int i = 0; i <= max_fd; i++) {
		if (FD_ISSET(i, &fds_read) && i != sockfd && i != fd) {
			send(i, msg, strlen(msg), 0); // TODO: error check missing
		}
	}

}

void	ft_remove_client(int sockfd, int fd) {
	FD_CLR(fd, &fds_read);
	close(fd);
	if (outbuf[fd]) {
		free(outbuf[fd]);
		outbuf[fd] = NULL;
	}
	char	byebye[1024];
	sprintf(byebye, "server: client %d just left\n", ids[fd]);
	ft_send_msg(sockfd, fd, byebye);
	DEBUG_PRINT("Client %d has left the server.\n", fd);
}

/**
 * @brief function to send a message from one client to all clients
 * incomplete message parts (that do not end with a new line are stored in the outbuf arrary)
 * socket fd and sender are excluded
 */
void ft_transmit_msg(int sockfd, int fd) {
	char	*to_print;
	while(extract_message(&outbuf[fd], &to_print)) { // TODO: error check missing
		ft_send_msg(sockfd, fd, to_print);
		free(to_print);
	}
	DEBUG_PRINT("New message from client received\n");
}

/**
 * @brief function to register a new client
 * the fd gets add to the select makro
 * the id gets added to the fds aray
 */
void	ft_register_new_client(int sockfd, int fd) {
	FD_ISSET(fd, &fds_read);
	outbuf[fd] = NULL;
	last_id += 1;
	ids[fd] = last_id;
	if (fd > max_fd)
		max_fd = fd;
	char	welcome[1024];
	sprintf(welcome, "server: client %d just arrived\n", ids[fd]);
	ft_send_msg(sockfd, fd, welcome);
	DEBUG_PRINT("New client registered with id %d\n", last_id);
}

int main(int argc, char *argv[]) {

	// check number of arguments
	if (argc != 2) {
		char error_msg[] = "Wrong number of arguments\n";
		write(2, error_msg, strlen(error_msg));
		exit (1);
	}

	//read port from arguments
	int	port = 0;
	port = atoi(argv[1]);
	if (port == 0) {
		DEBUG_PRINT("Port number incorrect\n");
		ft_err_exit(0);
	}

	// not required by subject, implemented to debug in a clean manner
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGPIPE, SIG_IGN);

	bzero(ids, sizeof(ids));
	FD_ZERO(&fds_read); //makro for select
	FD_ZERO(&fds_write); //makro for select
	bzero(ids, sizeof(ids));
	bzero(outbuf, sizeof(outbuf));

	//adapted part from pre-given main START
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		DEBUG_PRINT("socket creation failed...\n"); 
		ft_err_exit(0);
	} 
	else
		DEBUG_PRINT("Socket successfully created..\n"); 

	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		DEBUG_PRINT("socket bind failed...\n"); 
		ft_err_exit(sockfd);
	} 
	else
		DEBUG_PRINT("Socket successfully binded..\n");
	if (listen(sockfd, 10) != 0) {
		DEBUG_PRINT("cannot listen\n"); 
		ft_err_exit(sockfd);
	}
	//adapted part from pre-given main END
	
	FD_SET(sockfd, &fds_read); //makro for select
	max_fd = sockfd;

	fd_set	fds_loop;

	while (g_signalnum == 0) {
		DEBUG_PRINT("Start main server loop\n");
		fds_loop = fds_read;
		if (select(max_fd + 1, &fds_loop, &fds_write, NULL, NULL) < 0) {
			DEBUG_PRINT("server select failed...\n");
			ft_err_exit(sockfd);
		}
		DEBUG_PRINT("Event on READ detected by select fct\n");
		for (int i = 0; i <= max_fd; i++) {
			if (FD_ISSET(i, &fds_loop)) {
				if (i == sockfd) { //CASE NEW CLIENT
					socklen_t	len;
					len = sizeof(cli);
					connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
					if (connfd < 0) { 
						DEBUG_PRINT("server acccept failed...\n"); 
						ft_err_exit(sockfd);
					} 
					else
						DEBUG_PRINT("server acccept the client...\n");
					ft_register_new_client(sockfd, connfd);
					break;
				}
				else {
					char	recv_buf[1024];
					int	bytes_received = recv(i, recv_buf, sizeof(recv_buf), 0);
					if (bytes_received <= 0) { //CASE CLIENT LOST
						ft_remove_client(sockfd, i);
						break;
					}
					else { //CASE NEW MESSAGE FROM CLIENT
					       	recv_buf[bytes_received] = '\0';
						outbuf[i] = str_join(outbuf[i], recv_buf); // TODO: error check missing
						ft_transmit_msg(sockfd, i);
						bzero(recv_buf, sizeof(recv_buf));
					}
				}
			}
		}
	}
	ft_err_exit(sockfd);
}
