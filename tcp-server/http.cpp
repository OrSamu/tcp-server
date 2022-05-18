#include "http.h"

void parseRequest(SocketState &socket) {
    char* buff = socket.buffer;
    bool hasQs = string(buff).find("?");
    char* token = strtok(buff, " ?");
    vector<string> headers;

    while (token != nullptr)
    {
        headers.push_back(string(token));
        cout << string(token) << endl;
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
    
    fileToCreate << "Request type is: " << socket.req.type << endl;
    fileToCreate << "Request path is: " << socket.req.path << endl;
    for (short i = 0; i < socket.req.qs.size(); i+=2)
    {
        fileToCreate << socket.req.qs[i] << endl;
    }
    fileToCreate.close();
}

