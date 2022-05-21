#include "http.h"

Request parseRequest(SocketState &socket) {
    Request req;

    string stringBuff = string(socket.buffer);
    short reqEnd = stringBuff.find("\r\n\r\n");
    char* reqBuff = (char*)malloc(reqEnd);

    if (reqEnd == -1) {
        return -1;
    }
    
    reqEnd += 4;
    strncpy(reqBuff, socket.buffer, reqEnd);    
    memcpy(socket.buffer, &socket.buffer[reqEnd], socket.len - reqEnd);
    socket.len -= reqEnd;
    bool hasQs = string(reqBuff).find("?");
    char* token = strtok(reqBuff, " :?\r\n");
    vector<string> headers;

    while (token != nullptr)
    {
        headers.push_back(string(token));
        cout << string(token) << endl;
        token = strtok(nullptr, " :?\r\n");
    }

    socket.req.type = headers[0];
    socket.req.path = headers[1];

    if(hasQs)
        breakQueryParams(socket.req.qs, headers[2]);

    for (int i = 0; i < headers.size(); i++) {
        if (headers[i].compare("Content-Length") == 0) {
            socket.req.contentLength = stoi(headers[i + 1]);
        }
    }

    int bodyLen = socket.req.contentLength;

    if (socket.req.contentLength > 0) {
        socket.req.body.append(stringBuff.substr(reqEnd, bodyLen));
        memcpy(socket.buffer, &socket.buffer[bodyLen], socket.len - bodyLen);
        socket.len -= bodyLen;
    }

    return 1;
}

Response handleRequests(Request& req) {
    Response res;

    if (req.type == GET) {
        handleGetRequest(req, res);
    }
    else if (req.type == PUT) {
        //handlePutReq(SocketState & socket);
    }
    else if (req.type == POST) {
    }
    else if (req.type == DELETEREQ) {
    }
    else if (req.type == TRACE) {
    }
    else if (req.type == HEAD) {
        handleHeadReq(req, res);
    }
    else if (req.type == OPTIONS) {
    }
}


void breakQueryParams(vector<string>& qs, string query) {
    char *q = (char*)malloc(query.length());

    if (!q)
        return;

    strcpy(q, query.c_str());
    char* token = strtok(q, "=&");

    while (token != nullptr)
    {
        qs.push_back(string(token));
        token = strtok(nullptr, "=&");
    }
}

string getFileName(vector<string> query, string path)
{
    short length = query.size();
    string fileName = "en";

    for (short i = 0; i < length; i = i + 2) {
        if (query[i].compare("lang") == 0) {
            fileName = query[i + 1];
        }
    }

    fileName.append("_");
    fileName.append(path.substr(1, path.length()));

    return fileName;
}

void handleGetRequest(SocketState& socket) {
    string       text;
    stringstream stream;
    SOCKET connected = socket.id;
    string fileName = getFileName(socket.req.qs, socket.req.path);
    string sendFile = htmlFileToStr(fileName);

    if (sendFile == "") /* check it the file was opened */
        return;


    long int totalBytes = sendFile.length();
    stream << "HTTP/1.1 200 OK\nContent-length: " << totalBytes << "\n" << "Content-Type: text/html\r\n\r\n";

    text = stream.str();
    /* you don't need a vector and strcpy to a char array, just call the .c_str() member
     * of the string class and the .length() member for it's length
     */
    send(connected, text.c_str(), text.length(), 0);

    int bytesSent = send(connected, sendFile.c_str(), (int)totalBytes, 0);
    if (SOCKET_ERROR == bytesSent)
    {
        cout << "HTTP Server: Error at send(): " << WSAGetLastError() << endl;
        return;
    }
}

void handlePutReq(SocketState& socket)
{
    string fileName = getFileName(socket.req.qs, socket.req.path);
    ofstream fileToCreate (fileName, ios_base::trunc);
    fileToCreate << socket.req.body;
    fileToCreate.close();
}

void handleHeadReq(Request& request, Response& response)
{
    bool isFileExist = false;
    string fileName = getFileName(socket.req.qs, socket.req.path);
    ifstream fileToCheck(fileName, ios_base::in);
    Response response;

    if (fileToCheck.is_open())
    {
        response.statusCode = OK;
        fileToCheck.close();
    }
    else
    {
        response.statusCode = NOT_FOUND;
    }

    response.body = "";
    response.bodyLength = 0;

    return; //responseToString(response);
}

string responseToString(Response response) {
    string responseString;

    responseString.append(response.httpVersion + " ");
    responseString.append(to_string(response.statusCode) + " ");

}