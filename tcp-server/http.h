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

enum StatusCode {	OK = 200, CREATED = 201, NO_CONTENT = 204,
					NOT_FOUND = 404, METHOD_NOT_ALLOWED = 405,
					UNSUPPORTED_MEDIA_TYPE = 415, INTERNAL_SERVER_ERROR = 500,
					NOT_IMPLEMENTED = 501, HTTP_VERSION_NOT_SUPPORTED = 505		};

struct Request
{
	string httpVersion;
	string type;
	string path;
	string body;
	int contentLength;
	map<string, string> qs;
};

struct Response
{
	const string httpVersion = "HTTP/1.1";
	StatusCode statusCode;
	string body;
	map<string, string> headers;
};

struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	Request req;		// Sending sub-type
	char buffer[4096];
	int len;
};

Request parseRequest(SocketState&);
Response handleRequests(Request& request);
void breakQueryParams(vector<string> &, string);
void handleGetRequest(SocketState& socket);
void handlePutReq(SocketState& socket);
void handleHeadReq(Request &request, Response &response);
string getFileName(vector<string> query, string path);
string htmlFileToStr(string fileName);
string responseToString(Response response);