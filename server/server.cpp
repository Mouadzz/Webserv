/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlasrite <mlasrite@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/19 18:51:55 by mlasrite          #+#    #+#             */
/*   Updated: 2021/11/22 12:44:19 by mlasrite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int entry()
{
    Server server;

    std::vector<int> ports;
    ports.push_back(8000);
    ports.push_back(8001);

    server.setPorts(ports);
    server.startServSockets();
    server.fillSockSet();
    while (1)
    {
        server.performSelect();
    }
    server.clean();

    system("leaks Webserv");
    return 0;
}