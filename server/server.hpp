/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlasrite <mlasrite@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 18:28:22 by mlasrite          #+#    #+#             */
/*   Updated: 2021/11/19 18:51:59 by mlasrite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Socket
{
private:
    int _sockfd;
    int _port;
    struct sockaddr_in _serv_addr;

public:
    Socket(int port) : _port(port) {}
    ~Socket() {}

    void m_create()
    {
        this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    }

    void m_config()
    {
        // assign  Ip, convert port to network byte order and assign local address
        this->_serv_addr.sin_family = AF_INET;
        this->_serv_addr.sin_port = htons(this->_port);
        this->_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    void m_bind()
    {
        bind(this->_sockfd, (struct sockaddr *)&this->_serv_addr, sizeof(this->_serv_addr));
    }

    void m_listen()
    {
        listen(this->_sockfd, 5);
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
