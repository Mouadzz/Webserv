/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlasrite <mlasrite@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 18:28:22 by mlasrite          #+#    #+#             */
/*   Updated: 2021/11/20 16:21:35 by mlasrite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"
#include <vector>
#define MAX_BUFFER_SIZE 1024 * 20

class Server
{
private:
    std::vector<Socket> _sockets;
    std::vector<int> _ports;
    fd_set _sockSet;
    int _maxFd;

public:
    Server() : _maxFd(0) {}
    ~Server() { this->clean(); }

    void setPorts(std::vector<int> &ports) { this->_ports = ports; }

    void startServSockets()
    {
        for (size_t i = 0; i < this->_ports.size(); i++)
        {
            Socket sock(true);
            sock.setPort(this->_ports[i]);
            this->_sockets.push_back(sock);
        }

        for (size_t i = 0; i < this->_sockets.size(); i++)
        {
            if (this->_sockets[i].isServSock())
                this->_sockets[i].launchSock();
        }
    }

    void fillSockSet()
    {
        // reset our set
        FD_ZERO(&this->_sockSet);

        // add sockets to the set
        for (size_t i = 0; i < this->_sockets.size(); i++)
        {
            FD_SET(this->_sockets[i].getSockFd(), &this->_sockSet);
            if (this->_sockets[i].getSockFd() > this->_maxFd)
                this->_maxFd = this->_sockets[i].getSockFd();
        }
    }

    void performSelect()
    {
        int result = select(this->_maxFd + 1, &this->_sockSet, NULL, NULL, NULL);
        if (result == -1)
            std::cout << "Error\n";
        else if (result > 0)
        {
            for (size_t i = 0; i < this->_sockets.size(); i++)
            {
                if (FD_ISSET(this->_sockets[i].getSockFd(), &this->_sockSet))
                {
                    if (this->_sockets[i].isServSock())
                        acceptNewClient(this->_sockets[i]);
                    else
                        handleClient(this->_sockets[i]);
                }
            }
        }
    }

    void acceptNewClient(Socket &sock)
    {
        std::cout << "New Client appeared on port: " << sock.getPort() << "\n";

        int newClient = accept(sock.getSockFd(), 0, 0);

        Socket client(false);
        client.setSockFd(newClient);
        this->_sockets.push_back(client);

        FD_SET(client.getSockFd(), &this->_sockSet);
        if (client.getSockFd() > this->_maxFd)
            this->_maxFd = client.getSockFd();
    }

    void handleClient(Socket &client)
    {
        std::cout << "Handle Client: " << client.getSockFd() << " !\n";
        char buff[MAX_BUFFER_SIZE];

        int size = recv(client.getSockFd(), buff, MAX_BUFFER_SIZE, 0);
        if (size <= 0)
            return;
        std::cout << "size -> " << size << std::endl;
        sendRequest(client);
        // FD_CLR(client.getSockFd(), &this->_sockSet);
        // client.m_close();
    }

    void sendRequest(Socket &client)
    {
        time_t t;
        time(&t);
        std::string body = ctime(&t);
        std::string headers = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
        headers.append(std::to_string(body.size()));
        headers.append("\n\n");
        char *response = strdup((headers + body).c_str());
        send(client.getSockFd(), response, strlen(response), 0);
    }

    void clean()
    {
        for (size_t i = 0; i < this->_sockets.size(); i++)
            this->_sockets[i].m_close();
        this->_sockets.clear();
    }
};

int entry();
