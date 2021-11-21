/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlasrite <mlasrite@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 18:28:22 by mlasrite          #+#    #+#             */
/*   Updated: 2021/11/21 12:58:47 by mlasrite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"
#include <vector>
#include <algorithm>
#define MAX_BUFFER_SIZE 1024 * 20

class Server
{
private:
    std::vector<Socket> _sockets;
    std::vector<int> _ports;
    fd_set _readSet;
    fd_set _writeSet;
    int _maxFd;

public:
    Server() : _maxFd(-1) {}
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

    void addToSet(int fd, fd_set &set)
    {
        FD_SET(fd, &set);
        if (fd > this->_maxFd)
            this->_maxFd = fd;
    }

    void deleteFromSet(int fd, fd_set &set) { FD_CLR(fd, &set); }

    void fillSockSet()
    {
        // reset our sets
        FD_ZERO(&this->_readSet);
        FD_ZERO(&this->_writeSet);

        // add server sockets to the read set
        for (size_t i = 0; i < this->_sockets.size(); i++)
            addToSet(this->_sockets[i].getSockFd(), this->_readSet);
    }

    void performSelect()
    {
        fd_set tmpReadSet = this->_readSet;
        fd_set tmpWriteSet = this->_writeSet;
        int result = select(this->_maxFd + 1, &tmpReadSet, &tmpWriteSet, NULL, NULL);
        std::cout << "after Select\n";
        if (result == -1)
            std::cout << "Error\n";
        else if (result > 0)
        {
            for (size_t i = 0; i < this->_sockets.size(); i++)
            {
                // check if a file descriptor is ready for read
                if (FD_ISSET(this->_sockets[i].getSockFd(), &tmpReadSet))
                {
                    if (this->_sockets[i].isServSock())
                        acceptNewClient(this->_sockets[i]);
                    else
                        handleClient(this->_sockets[i]);
                }

                // check if a file descriptor is ready for write
                if (FD_ISSET(this->_sockets[i].getSockFd(), &tmpWriteSet))
                    sendRequest(this->_sockets[i]);
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
        addToSet(client.getSockFd(), this->_readSet);
    }

    void handleClient(Socket &client)
    {
        std::cout << "Handle Client: " << client.getSockFd() << " !\n";
        char buff[MAX_BUFFER_SIZE];

        int size = recv(client.getSockFd(), buff, MAX_BUFFER_SIZE, 0);
        if (size <= 0)
            return;
        deleteFromSet(client.getSockFd(), this->_readSet);
        addToSet(client.getSockFd(), this->_writeSet);
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
        if (client.keepAlive())
        {
            std::cout << "Keep client: " << client.getSockFd() << " alive.\n";
            deleteFromSet(client.getSockFd(), this->_writeSet);
            addToSet(client.getSockFd(), this->_readSet);
        }
        else
        {
            std::cout << "Dont keep client: " << client.getSockFd() << " alive.\n";
            deleteFromSet(client.getSockFd(), this->_writeSet);
            client.m_close();
            std::vector<Socket>::iterator position = std::find(this->_sockets.begin(), this->_sockets.end(), client);
            if (position != this->_sockets.end())
                this->_sockets.erase(position);
        }
    }

    void clean()
    {
        for (size_t i = 0; i < this->_sockets.size(); i++)
            this->_sockets[i].m_close();
        this->_sockets.clear();
    }
};

int entry();
