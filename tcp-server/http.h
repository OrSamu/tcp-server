#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string.h> 
#include <iostream>
using namespace std;
#include <vector> 
#include <winsock2.h>

struct Request
{
	string type;
	string path;
	vector<string> qs;
};

struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	Request req;	// Sending sub-type
	char buffer[128];
	int len;
};

void breakQueryParams(vector<string> &, string);
void parseRequest(SocketState&);