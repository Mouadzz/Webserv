/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlasrite <mlasrite@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 18:51:55 by mlasrite          #+#    #+#             */
/*   Updated: 2021/11/19 19:27:15 by mlasrite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int ports[] = {8000,
               8001,
               8002,
               8003};

void launchServSocks(std::vector<Socket> &servSocks)
{
    for (size_t i = 0; ports[i]; i++)
        servSocks.push_back(Socket(ports[i]));

    for (size_t i = 0; i < servSocks.size(); i++)
        servSocks[i].launchSock();
}

void clean(std::vector<Socket> &servSocks)
{
    for (size_t i = 0; i < servSocks.size(); i++)
        servSocks[i].m_close();
    servSocks.clear();
}

void fillSockSet(fd_set &sockSet, std::vector<Socket> &servSocks, std::vector<int> &clientSocks)
{
    // reset our set
    FD_ZERO(&sockSet);

    // add serv sockets
    for (size_t i = 0; i < servSocks.size(); i++)
        FD_SET(servSocks[i].getSockFd(), &sockSet);

    // add client sockets
    for (size_t i = 0; i < clientSocks.size(); i++)
        FD_SET(clientSocks[i], &sockSet);
}

void acceptNewClient(Socket servSock, std::vector<int> &clientSocks)
{
    int newClient;

    std::cout << "New Client appeared on port: " << servSock.getPort() << "\n";
    newClient = accept(servSock.getSockFd(), 0, 0);
    clientSocks.push_back(newClient);
}

void handleClient(int clientSock)
{
    std::cout << "Handle Client: " << clientSock << " !\n";
    time_t t;
    time(&t);
    std::string body = ctime(&t);
    std::string headers = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
    headers.append(std::to_string(body.size()));
    headers.append("\n\n");
    char *response = strdup((headers + body).c_str());
    send(clientSock, response, strlen(response), 0);
}

void entry()
{
    std::vector<Socket> servSocks;
    std::vector<int> clientSocks;
    fd_set sockSet;
    int result;

    launchServSocks(servSocks);
    while (1)
    {
        fillSockSet(sockSet, servSocks, clientSocks);
        result = select(FD_SETSIZE, &sockSet, NULL, NULL, NULL);
        if (result == -1)
            std::cout << "Error\n";
        else if (result > 0)
        {
            // test serv sockets
            for (size_t i = 0; i < servSocks.size(); i++)
            {
                if (FD_ISSET(servSocks[i].getSockFd(), &sockSet))
                    acceptNewClient(servSocks[i], clientSocks);
            }

            // Now test the client sockets
            for (size_t i = 0; i < clientSocks.size(); i++)
            {
                if (FD_ISSET(clientSocks[i], &sockSet))
                    handleClient(clientSocks[i]);
            }
        }
    }
    clean(servSocks);
}