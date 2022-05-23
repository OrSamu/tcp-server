#include "http.h"

int parseRequest(SocketState &socket) {

    socket.req.state = PARTIAL_LOAD;

    if (socket.req.method.compare("") == 0) {
        int headerError = parseHeaders(socket);

        if (headerError == -1)
            return -1;
    }

    if (socket.req.contentLength > 0 &&
        socket.req.body.length() != socket.req.contentLength) {
        parseBody(socket);

        if (socket.req.body.length() != socket.req.contentLength)
            return -1;
    }

    socket.req.state = FINISH_LOAD;
    return 1;
}

int parseHeaders(SocketState& socket) {
    string stringBuff = string(socket.buffer);
    short reqEnd = stringBuff.find("\r\n\r\n");
    char reqBuff[4096];

    if (reqEnd == -1) {
        return -1;
    }

    vector<string> headers;
    reqEnd += 4;
    strncpy(reqBuff, socket.buffer, reqEnd);
    memcpy(socket.buffer, &socket.buffer[reqEnd], socket.len - reqEnd);
    socket.len -= reqEnd;
    socket.buffer[socket.len] = '\0';
    bool hasQs = string(reqBuff).find("?");
    
    char* token = strtok(reqBuff, " :?\r\n");

    while (token != nullptr)
    {
        headers.push_back(string(token));
        token = strtok(nullptr, " :?\r\n");
    }

    socket.req.method = headers[0];
    socket.req.path = headers[1];

    if (hasQs)
        breakQueryParams(socket.req.qs, headers[2]);

    for (int i = 0; i < headers.size(); i++) {
        if (headers[i].compare("Content-Length") == 0) {
            socket.req.contentLength = stoi(headers[i + 1]);
        }
    }

    return 1;
}

void parseBody(SocketState& socket) {
    string stringBuff = string(socket.buffer);
    int remaining = socket.req.contentLength - socket.req.body.length();
    int available = socket.len;
    int bodyLen = min(remaining, available);

    if (bodyLen > 0) {
        socket.req.body.append(stringBuff.substr(0, bodyLen));
        memcpy(socket.buffer, &socket.buffer[bodyLen], socket.len - bodyLen);
        socket.len -= bodyLen;
        socket.buffer[socket.len] = '\0';
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

void updateFile(SocketState& socket)
{
    string fileName = getFileName(socket.req.qs, socket.req.path);
    ofstream fileToCreate (fileName, ios_base::trunc);
    fileToCreate << socket.req.body;
    fileToCreate.close();
}

Response handleRequest(Request req) {
    StatusCode requestStatusCode = validateRequest(req);
    Response res;

    if (requestStatusCode != OK) {
        res.statusCode = requestStatusCode;
        res.reasonPhrase = "Method Not Allowed";
    }
    else {
        if (req.method == OPTIONS)
        {
            //handleOptionsRequest(request, response);
        }
        else if (req.method == GET)
        {
            handleGetRequest(req, res);
        }
        else if (req.method == HEAD)
        {
            //handleHeadRequest(request, response);
        }
        else if (req.method == POST)
        {
            //handlePostRequest(request, response);
        }
        else if (req.method == PUT)
        {
            handlePutRequest(req, res);
        }
        else if (req.method == DELETEREQ)
        {
            //handleDeleteRequest(request, response);
        }
        else if (req.method == TRACE)
        {
            //handleTraceRequest(request, response);
        }
    }

    return res;
}

StatusCode validateRequest(Request& req) {
    if (!isAllowedMethod(req.method))
    {
        return METHOD_NOT_ALLOWED;
    }

    return OK;
}

bool isAllowedMethod(string method)
{
    return method == OPTIONS || method == GET || method == HEAD || method == POST || method == PUT || method == DELETEREQ || method == TRACE;
}

void handleGetRequest(Request &req, Response& res) {
    string fileName = getFileName(req.qs, req.path);
    string sendFile = htmlFileToStr(fileName);

    if (sendFile == "") {   /* check it the file was opened */
        res.statusCode = NOT_FOUND;
        res.reasonPhrase = "File Not Found";
    }
    else {
        res.statusCode = OK;
        res.reasonPhrase = "OK";
        res.messageBody = sendFile;
        res.bodyLength = sendFile.length();
    }
}

void handlePutRequest(Request& req, Response& res) {
    string fileName = getFileName(req.qs, req.path);
    ifstream readFile(fileName,ios_base::in);
    ofstream writeFile;

    if (readFile.is_open()) {
        readFile.close();
        res.statusCode = OK;
        res.reasonPhrase = "OK";
    }
    else {
        res.statusCode = CREATED;
        res.reasonPhrase = "Created";
    }

    writeFile.open(fileName, ios_base::trunc);
    writeFile << requestToString(req) << endl;
    writeFile.close();

}

string htmlFileToStr(string fileName) {
    string res, curLine;
    ifstream file(string("./").append(fileName));

    while (getline(file, curLine)) {
        res.append(curLine);
    }

    file.close();
    return res;
}

string responseToString(Response res) {
    string responseString;
    
    responseString.append(res.httpVersion + " ");
    responseString.append(to_string(res.statusCode) + " ");
    responseString.append(res.reasonPhrase + "\n");

    time_t timeT = time(NULL);

    responseString.append("Date: " + string(ctime(&timeT)));
    responseString.append("Cache-Control: no-cache\n");
    responseString.append("Content-Length: " + to_string(res.bodyLength) + "\n");

    string contentType = res.headers["Content-Type"].empty() ? "text/html" : res.headers["Content-Type"];

    responseString.append("Content-Type: " + contentType + "\n");
    if (!res.headers["Allow"].empty())
    {
        responseString.append("Allow: " + res.headers["Allow"] + "\n");
    }

    responseString.append("\r\n" + res.messageBody);

    return responseString;
}

string requestToString(Request& req) {
    string requestString;

    requestString.append(req.method + " ");
    requestString.append(req.path + "\n");

    requestString.append("Content Length: " + to_string(req.contentLength) + "\n");
    if (req.contentLength > 0) {
        requestString.append("Body:\n" + req.body);
    }

    return requestString;
}