#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <vector>
#include <string>
#include "common.h"
#include "functions.h"

class Core : public QObject
{
    Q_OBJECT
    Q_ENUMS(ret)
    Q_PROPERTY(QString ip_addr READ ip_addr WRITE set_ip_addr)
    Q_PROPERTY(int port READ port WRITE set_port)
    Q_PROPERTY(int passive READ passive WRITE set_passive)
    Q_PROPERTY(QString defaultdir READ defaultdir WRITE set_defaultdir)
    Q_PROPERTY(QString ClientDir READ ClientDir WRITE set_ClientDir)
    Q_PROPERTY(QString ServerDir READ ServerDir WRITE set_ServerDir)
    Q_PROPERTY(int listnumber READ listnumber)

public:
    Core(QObject* parent = 0);
    virtual ~Core() = default;

    Q_INVOKABLE void set_ip_addr(const QString& param) {ip_addr_ = param;}
    Q_INVOKABLE void set_port(const int param){port_ = param;}
    Q_INVOKABLE void set_passive(const int param){passive_ = param;}
    Q_INVOKABLE void set_defaultdir(const QString& param){defaultdir_ = param;}
    Q_INVOKABLE void set_ClientDir(const QString& param){ClientDir_ = param;}
    Q_INVOKABLE void set_ServerDir(const QString& param){ServerDir_ = param;}
    Q_INVOKABLE void set_DownLoadPath(const QString& param) { DownLoadLocation = param; }

    QString ip_addr() { return ip_addr_;}
    int port() {return port_;}
    int passive() {return passive_;}
    QString defaultdir() { return defaultdir_;}
    QString ClientDir() { return ClientDir_;}
    QString ServerDir() { return ServerDir_;}
    QString DownLoadPath() { return DownLoadLocation;}
    int listnumber() { return listnumber_; }
    enum ret
    {
        OK,
        NOT_LOGIN,
        ERROR_CREATE_SOCKET,
        ERROR_BIND_SOCKET,//3
        WRONG_IP,
        ERROR_CONNECT, //5
        CONNECTED,
        LOGIN_FAIL,
        NOT_CONNECTED, //8
        LOGGED,
        ERROR_DATA_CONNECTION,
        NO_REFRESH,
        NOT_DOWNLOADING, //12
        ROOT_DIR,
        FILE_NOT_FOUND,
        ERROR_WRITING, //15
        IS_DIR
    };
public slots:
    Q_INVOKABLE ret login();// user name: ClientDir, passwd: ServerDir
    Q_INVOKABLE ret get(QString target);
    Q_INVOKABLE ret upload();
    Q_INVOKABLE ret rename();
    Q_INVOKABLE ret disconnect();
    Q_INVOKABLE ret connect();
    Q_INVOKABLE ret mkd();
    Q_INVOKABLE ret rmd();
    Q_INVOKABLE ret logout();
    Q_INVOKABLE void toggle();
    Q_INVOKABLE ret renewList();
    Q_INVOKABLE QString _Name(int i);
    Q_INVOKABLE QString _Type(int i);
    Q_INVOKABLE ret cd(QString foldername);
    Q_INVOKABLE ret abort();
    Q_INVOKABLE void _continue(QString t);
private:
    QString ip_addr_;
    int port_;
    bool passive_;
    QString defaultdir_;
    QString ClientDir_;
    QString ServerDir_;
    QString CurrentPath;
    QString DownLoadLocation;
    QString target_;

    int sockfd;
    struct sockaddr_in addr;
    char sentence[8192];
    int len;
    int p;
    bool downloading;
public:
    bool connected;
    bool logged;
    bool m_passive;
    std::vector<std::string> name;
    std::vector<std::string> type;
    int listnumber_;
};

#endif // MESSAGE_H
