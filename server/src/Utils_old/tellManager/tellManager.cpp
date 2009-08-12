/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set sw=4 ts=4 et: */
/* ***** BEGIN LICENSE BLOCK *****
 *  Version: GPL 3
 * 
 *  This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 *  This program is licensed under the GNU General Public license, version 3 (GPL).
 * 
 *  This program is distributed in the hope that it will be useful, but
 *  AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 *  NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 *  is prohibited.
 * 
 *  You can redistribute and/or modify this program under the terms of the GPL, 
 *  as published by the Free Software Foundation.  You should
 *  have received a copy of the GNU General Public License, Version 3 along
 *  with this program; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *  
 *  Aptana provides a special exception to allow redistribution of this file
 *  with certain other code and certain additional terms
 *  pursuant to Section 7 of the GPL. You may view the exception and these
 *  terms on the web at http://www.aptana.com/legal/gpl/.
 *  
 *  You may view the GPL, and Aptana's exception and additional terms in the file
 *  titled license-jaxer.html in the main distribution folder of this program.
 *  
 *  Any modifications to this file must keep this entire header intact.
 * 
 * ***** END LICENSE BLOCK ***** */
// tellManager.cpp : Defines the entry point for the console application.
//

// #include "stdafx.h"
#include <stdio.h>

#ifdef WIN32
#include <winsock2.h>
typedef SOCKET socket_t;


#else
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#define SOCKET_ERROR (-1)
#define SOCKADDR sockaddr
typedef int socket_t;
#endif

#define BUFSIZE 0x8000

void CleanUp(socket_t sock)
{
#ifdef WIN32
    WSACleanup();
#else
    close(sock);
#endif
}

int main(int argc, char* argv[])
{
    int nPort = 4328;
    char cmd[BUFSIZE] ="";
    char recvbuf[BUFSIZE] ="";
    int bytesRecv = SOCKET_ERROR;
    int ret = -1;

    for (int i=1; i<argc; i++)
    {
        if ( strcmp(argv[i], "-p") == 0)
        {
            i++;
            nPort = atoi(argv[i]);
        }else
        {
            strcat(cmd, argv[i]);
            strcat(cmd, " ");
        }
    }

    strcat(cmd, "\n");
    
    int len = strlen(cmd);
    if (len <= 2)
        return -1;

#ifdef WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != NO_ERROR)
    {
        // printf("Error at WSAStartup().\n");
        return -1;
    }

#endif

  
    socket_t sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#ifdef WIN32
    if (sock == INVALID_SOCKET)
    {
        WSACleanup();
        return -2;
    }
#else
    if (sock < 0)
    {
        return -2;
    }
#endif
    
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(nPort);
    
    if (connect(sock, (SOCKADDR*)&clientService, sizeof(clientService)) != SOCKET_ERROR)
    {
        // receive Manager connection info and ignore
        if (recv(sock, recvbuf, BUFSIZE, 0) > 0)
        {
            if(send(sock, cmd, len, 0) == len)
            {
                bytesRecv = recv(sock, recvbuf, BUFSIZE, 0);
                if (bytesRecv > 0)
                {
                    recvbuf[bytesRecv] = 0;
                    printf(recvbuf);
                    fflush(stdout);
                    ret = 0;
                }
            }
        }
    }
    
    CleanUp(sock);

    return ret;
}

