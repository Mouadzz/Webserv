/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlasrite <mlasrite@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 18:51:55 by mlasrite          #+#    #+#             */
/*   Updated: 2021/11/19 18:51:58 by mlasrite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#define MAX_BUFFER_SIZE 1024 * 20

int handle_client(int fd)
{
    time_t t;
    time(&t);
    std::string body = ctime(&t);
    std::string headers = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
    headers.append(std::to_string(body.size()));
    headers.append("\n\n");
    char *response = strdup((headers + body).c_str());
    char buff[MAX_BUFFER_SIZE];
    int size;

    size = recv(fd, buff, MAX_BUFFER_SIZE, 0);
    if (size > 0)
    {
        std::cout << buff << std::endl;
    }
    int ret = send(fd, response, strlen(response), 0);
    return ret;
}

void accept_client(Socket sock)
{
    int connfd;
    connfd = accept(sock.getSockFd(), 0, 0);

    int code = handle_client(connfd);
    std::cout << code << std::endl;
    if (code != -1)
    {
        close(connfd);
        accept_client(sock);
    }
}

void entry()
{
    Socket sock(6556);
    sock.m_create();

    sock.m_config();

    sock.m_bind();

    sock.m_listen();

    accept_client(sock);

    sock.m_close();
}
