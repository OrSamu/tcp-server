#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h> 
#include <winsock2.h>
#include <vector> 

using namespace std;

const string GET = "GET";
const string PUT = "PUT";
const string POST = "POST";
const string DELETEREQ = "DELETE";
const string TRACE = "TRACE";
const string HEAD = "HEAD";
const string OPTIONS = "OPTIONS";

struct Request
{
	string type;
	string path;
	string body;
	int contentLength=0;
	vector<string> qs;
};

struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	Request req;	// Sending sub-type
	char buffer[4096];
	int len;
};

void breakQueryParams(vector<string> &, string);
int parseRequest(SocketState&);
void updateFile(SocketState& socket);
string getFileName(vector<string> query, string path);