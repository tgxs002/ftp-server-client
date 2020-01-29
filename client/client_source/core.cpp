#include "core.h"
#include "functions.h"
#include "common.h"
#include <QDebug>

int startWith(char* source, char* pattern)
{
    int len = strlen(pattern);
    for (int i = 0; i < len; i++)
    {
        if (source[i] != pattern[i])
        {
            return 0;
        }
    }
    return 1;
}

char reply[1000];
int receiving = 1;

void Q2C(QString& src, char* dst)
{
    QByteArray ba = src.toLatin1();
    strcpy(dst, ba.data());
}

Core::Core(QObject* parent) : QObject(parent), connected(false), m_passive(false), logged(false), downloading(false)
{

}

Core::ret Core::connect()
{
    if (connected)
    {
        return CONNECTED;
    }

    /* socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        return ERROR_CREATE_SOCKET;
    }

    /* port  */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);

    /* address */
    char add[30];
    Q2C(ip_addr_, add);
    if (inet_pton(AF_INET, add, &addr.sin_addr) <= 0) {
        return WRONG_IP;
    }

    /* try connect */
    if (::connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        return ERROR_CONNECT;
    }
    connected = true;
    return OK;
}

Core::ret Core::login()
{
    if (!connected)
    {
        return NOT_CONNECTED;
    }
    if (logged)
    {
        return LOGGED;
    }
    /* login */
    char user[50];
    Q2C(ClientDir_, user);
    char passwd[200];
    Q2C(ServerDir_, passwd);
    if (::login(sockfd, user, passwd))
    {
        logged = true;
        return OK;
    }
    else
    {
        return LOGIN_FAIL;
    }
}
Core::ret Core::get(QString target)
{
    if (!logged)
    {
        return NOT_LOGIN;
    }
    target_ = target;
    char getpath[200];
    Q2C(target, getpath);
    char savepath[200];
    Q2C(this->defaultdir_, savepath);
    download(getpath, sockfd, !passive_, savepath, downloading);
    return OK;
}
Core::ret Core::upload()
{
    if (!logged)
    {
        return NOT_LOGIN;
    }
    char o[200];
    Q2C(defaultdir_, o);
    char h[200];
    char* p = o + strlen(o);
    while (*p != '/') p--;
    p++;
    strcpy(h, p);
    strcat(h, "\n");
    int ret = _send(sockfd, o, h, !passive_);
    if (ret == 0)
    {
        return OK;
    }
    else if (ret == 1)
    {
        return FILE_NOT_FOUND;
    }
    else if (ret == 2)
    {
        return ERROR_DATA_CONNECTION;
    }
    else if (ret == 3)
    {
        return ERROR_WRITING;
    }
    else if (ret == 4)
    {
        return IS_DIR;
    }
}
Core::ret Core::rename()
{
    return OK;
}

Core::ret Core::disconnect()
{
    if (logged)
    {
        writeToSocket(sockfd, "QUIT\r\n", 100);
    }
    close(sockfd);
    logged = false;
    connected = false;
    return OK;
}
Core::ret Core::mkd()
{
    return OK;
}
Core::ret Core::rmd()
{
    return OK;
}

Core::ret Core::logout()
{
    if (logged)
    {
        writeToSocket(sockfd, "QUIT\r\n", 100);
        logged = false;
        return OK;
    }
    else
    {
        return NOT_LOGIN;
    }
}

void Core::toggle()
{
    m_passive = !m_passive;
}

Core::ret Core::renewList()
{
    char recvbuffer[200];
    int dataconnfd;
    if (!m_passive)
    {
        int listenfd = positive(sockfd);
        if ((dataconnfd = accept(listenfd, NULL, NULL)) == -1) {
            return ERROR_DATA_CONNECTION;
        }
        readFromSocket(sockfd, recvbuffer, 200);
        writeToSocket(sockfd, "LIST \n", 0);
        readFromSocket(sockfd, recvbuffer, 200);
    }
    else
    {
        dataconnfd = ::passive(sockfd);
        if (dataconnfd == -1)
            return ERROR_DATA_CONNECTION;
        writeToSocket(sockfd, "LIST \n", 0);
        readFromSocket(sockfd, recvbuffer, 200);
    }
    char resp[8192];
    readFromSocket(dataconnfd, resp, 8192);
    readFromSocket(sockfd, recvbuffer, 200);
    char* p = resp;
    listnumber_ = 0;
    while (*p != '\n') p++;
    p++;
    name.clear();
    type.clear();
    char name_temp[200];
    char type_temp[20];
    while (sscanf(p, "name: %s , type: %s", name_temp, type_temp) == 2)
    {
        if (strlen(name_temp) != 1 || name_temp[0] != '.')
        {
            name.push_back(std::string(name_temp));
            type.push_back(std::string(type_temp));
            listnumber_++;
        }
        while (*p != '\n') p++;
        p++;
    }
    close(dataconnfd);
    return OK;
}

QString Core::_Name(int i)
{
    return QString(name[i].data());
}

QString Core::_Type(int i)
{
    return QString(type[i].data());
}

Core::ret Core::cd(QString foldername)
{
    if (logged)
    {
        char folder[200];
        if (foldername != QString("root"))
        {
            bool found = false;
            for (int i = 0; i < name.size(); i++)
            {
                if (foldername == QString(name[i].data()))
                {
                    found = true;
                    if (type[i] == std::string("file"))
                    {
                        printf("is a file\n");
                        return NO_REFRESH;
                    }
                    break;
                }
            }
            if (!found)
            {
                printf("name not found\n");
                return NO_REFRESH;
            }
        }
        Q2C(foldername, folder);
        if (strlen(folder) == 1 && folder[0] == '.')
        {
            printf("is dot.\n");
            return NO_REFRESH;
        }
        char buffer[200];
        strcpy(buffer, "CWD ");
        strcat(buffer, folder);
        strcat(buffer, "\r\n");
        writeToSocket(sockfd, buffer, 0);
        readFromSocket(sockfd, buffer, 200);
        if (!startWith(buffer, "2") && foldername != QString("root"))
        {
            printf("server not found the dir.\n");
            return ROOT_DIR;
        }
        renewList();
        printf("%s\n", buffer);
        printf("cd ok, and refresh.\n");
        return OK;
    }
    else
    {
        printf("not logged in\n");
        return NOT_LOGIN;
    }
}

Core::ret Core::abort()
{
    if (logged && downloading)
    {

        writeToSocket(sockfd, "ABOR\r\n", 0);
        char buffer[200];
        readFromSocket(sockfd, buffer, 200);
        return Core::OK;
    }
    else
    {
        return Core::NOT_DOWNLOADING;
    }
}

void Core::_continue(QString t)
{
    if (t == target_)
    {
        char dir[200];
        Q2C(defaultdir_, dir);
        char name[200];
        Q2C(t, name);
        strcat(dir, "/");
        strcat(dir, name);
        struct stat statbuf;
        stat(dir, &statbuf);
        int size = statbuf.st_size;
        char cmd[20];
        sprintf(cmd, "REST %d\r\n", size);
        writeToSocket(sockfd, cmd, 0);
        readFromSocket(sockfd, dir, 200);
        char savepath[200];
        Q2C(this->defaultdir_, savepath);
        download(name, sockfd, !passive_, savepath, downloading, "at+");
    }
}
