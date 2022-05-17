#include "http.h"

void parseRequest(SocketState &socket) {
    char* buff = socket.buffer;
    bool hasQs = string(buff).find("?");
    char* token = strtok(buff, " ?");
    vector<string> headers;

    while (token != nullptr)
    {
        headers.push_back(string(token));
        token = strtok(nullptr, " ?");
    }

    socket.req.type = headers[0];
    socket.req.path = headers[1];

    if(hasQs)
        breakQueryParams(socket.req.qs, headers[2]);
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

