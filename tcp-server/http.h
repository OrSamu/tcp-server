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

const short START_LOAD = 0;
const short PARTIAL_LOAD = 1;
const short FINISH_LOAD = 2;

struct Request
{
	string method;
	string path;
	string body;
	short state = START_LOAD;
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
	time_t lastInputTime;
	int len;
};

void breakQueryParams(vector<string> &, string);
int parseRequest(SocketState&);
int parseHeaders(SocketState&);
void parseBody(SocketState&);
void updateFile(SocketState&);
string getFileName(vector<string>, string);