#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h> 
#include <winsock2.h>
#include <vector> 
#include <map>

using namespace std;

const string GET = "GET";
const string PUT = "PUT";
const string POST = "POST";
const string DELETEREQ = "DELETE";
const string TRACE = "TRACE";
const string HEAD = "HEAD";
const string OPTIONS = "OPTIONS";

const short START_LOAD = 0;
const short FINISH_LOAD = 1;

enum StatusCode {	OK = 200, CREATED = 201, NO_CONTENT = 204,
					NOT_FOUND = 404, METHOD_NOT_ALLOWED = 405,
					UNSUPPORTED_MEDIA_TYPE = 415, INTERNAL_SERVER_ERROR = 500,
					NOT_IMPLEMENTED = 501, HTTP_VERSION_NOT_SUPPORTED = 505
};

struct Request
{
	string method;
	string path;
	string body;
	short state = START_LOAD;
	int contentLength=0;
	vector<string> qs;
};

struct Response
{
	string httpVersion = "HTTP/1.1";
	StatusCode statusCode;
	string reasonPhrase;
	map<string, string> headers;
	string messageBody = "";
	int bodyLength = 0;
};

struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	Request req;		// Sending sub-type
	char buffer[4096];
	time_t lastInputTime;
	int len;
};

int parseRequest(SocketState&);
int parseHeaders(SocketState&);
void parseBody(SocketState&);
void breakQueryParams(vector<string> &, string);
void updateFile(SocketState&);
void handleGetRequest(Request& req, Response& res);
void handlePutRequest(Request& req, Response& res);
bool isAllowedMethod(string method);
string htmlFileToStr(string fileName);
string getFileName(vector<string>, string);
string responseToString(Response res);
string requestToString(Request& req);
StatusCode validateRequest(Request& req);
Response handleRequest(Request req);