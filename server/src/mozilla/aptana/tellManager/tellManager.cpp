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
#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
typedef SOCKET socket_t;


#else
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#define SOCKET_ERROR (-1)
#define SOCKADDR sockaddr
typedef int socket_t;
#include <errno.h>

int WSAGetLastError()
{
	return errno;
}
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

void DisplayUsage(char *argv0)
{
	fprintf(stdout, "Usage: %s [-h hostip] [-p port] <cmd>\n", argv0);
	fprintf(stdout, "Example (get all manager settings):\n\t %s get all\n", argv0);
	fprintf(stdout, "Example (change manager loglevel to TRACE):\n\t %s -h localhost set loglevel TRACE\n", argv0);
}

int main(int argc, char* argv[])
{
    int nPort = 4328;
	char *sHost = "127.0.0.1";
    char cmd[BUFSIZE] ="";
    char recvbuf[BUFSIZE] ="";
    int bytesRecv = SOCKET_ERROR;
    int ret = -1;
	int i;

    for (i=1; i<argc; i++)
    {
        if ( strcmp(argv[i], "-p") == 0)
        {
            i++;
            nPort = atoi(argv[i]);
        }else if ( strcmp(argv[i], "-h") == 0)
        {
            i++;
            sHost = argv[i];
        }else if ( strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "-?") == 0)
        {
			DisplayUsage(argv[0]);
			exit(1);
        }else
		{
			break;
		}
	}

	for(; i<argc; i++)
    {
        strcat(cmd, argv[i]);
        strcat(cmd, " ");
    }
	strcat(cmd, "\n");
    
    int len = strlen(cmd);
    if (len <= 2)
	{
		fprintf(stderr, "No cmd specified\n");
        DisplayUsage(argv[0]);
		return -1;
	}

    

#ifdef WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        fprintf(stderr, "Error at WSAStartup().\n");
        return -1;
    }
#endif
 
    socket_t sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#ifdef WIN32
    if (sock == INVALID_SOCKET)
    {
		fprintf(stderr, "socket failed with error %d.\n", WSAGetLastError());
        WSACleanup();
        return -2;
    }
#else
    if (sock < 0)
    {
		fprintf(stderr, "socket failed with error %d.\n", errno);
        return -2;
    }
#endif
    
	if (inet_addr(sHost) == (unsigned long)-1)
	{
		fprintf(stderr, "Cannot determine host (%s) address. Using 127.0.0.1\n", sHost);
		sHost= "127.0.0.1";
	}
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(sHost);
    clientService.sin_port = htons(nPort);
    
    if (connect(sock, (SOCKADDR*)&clientService, sizeof(clientService)) != SOCKET_ERROR)
    {
        // receive Manager connection info and ignore
        if (recv(sock, recvbuf, BUFSIZE, 0) > 0)
        {
			// assume one send is enough.  Should make it work properly.
            if(send(sock, cmd, len, 0) == len)
            {
                if ( (bytesRecv = recv(sock, recvbuf, BUFSIZE, 0))>0 )
                {
                    recvbuf[bytesRecv] = 0;
                    printf(recvbuf);
                    fflush(stdout);
                    ret = 0;
                }else
				{
					fprintf(stderr, "recv failed with error %d.\n", WSAGetLastError());
					ret = -1;
				}
            }else
			{
				fprintf(stderr, "send failed with error %d.\n", WSAGetLastError());
				ret = -1;
			}
        }
	    else
		{
			fprintf(stderr, "recv failed with error %d.\n", WSAGetLastError());
			ret = -1;
		}
    }else
	{
		fprintf(stderr, "connect failed with error %d.\n", WSAGetLastError());
		ret = -1;
	}
    
    CleanUp(sock);

    return ret;
}

