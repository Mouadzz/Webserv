/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlasrite <mlasrite@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 18:28:22 by mlasrite          #+#    #+#             */
/*   Updated: 2021/11/19 19:19:55 by mlasrite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#define QUEUE_SIZE 10
#define MAX_BUFFER_SIZE 1024 * 20

class Socket
{
private:
    int _sockfd;
    int _port;
    struct sockaddr_in _serv_addr;

public:
    Socket(int port) : _port(port) {}
    ~Socket() {}

    void launchSock()
    {
        // create socket
        this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);

        // assign  Ip, convert port to network byte order and assign local address
        this->_serv_addr.sin_family = AF_INET;
        this->_serv_addr.sin_port = htons(this->_port);
        this->_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        // asign address to socket
        bind(this->_sockfd, (struct sockaddr *)&this->_serv_addr, sizeof(this->_serv_addr));

        // prepare the server for incoming clients requests
        listen(this->_sockfd, QUEUE_SIZE);
        std::cout << "Server listening on port: " << this->_port << std::endl;
    }

    void m_close()
    {
        close(this->_sockfd);
    }

    int getSockFd() { return this->_sockfd; }
    int getPort() { return this->_port; }
};

void entry();
