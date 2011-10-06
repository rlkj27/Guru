#ifndef QRESTCLIENT_H
#define QRESTCLIENT_H

#include <QSslSocket>
#include <QStringList>
#include <QMap>
#include <QThread>

typedef enum
{
    GET, POST, PUT, DELETE
} QRestMethod;

typedef enum
{
    UNASSIGNED = 0,
    INFO_CONTINUE = 100,
    INFO_SWITCHING_PROTOCOLS = 101,
    INFO_PROCESSING = 102,
    INFO_CHECKPOINT = 103,
    INFO_REQUEST_URI_TOO_LONG = 122,
    OK_OK = 200,
    OK_CREATED = 201,
    OK_ACCEPTED = 202,
    OK_NONAUTHORITATIVE_INFO = 203,
    OK_NO_CONTENT = 204,
    OK_RESET_CONTENT = 205,
    OK_PARTIAL_CONTENT = 206,
    OK_MULTI_STATUS = 207,
    OK_IM_USED = 226,
    REDIR_MULTI_CHOICE = 300,
    REDIR_MOVED_PERMANENTLY = 301,
    REDIR_FOUND = 302,
    REDIR_SEE_OTHER = 303,
    REDIR_NOT_MODIFIED = 304,
    REDIR_USE_PROXY = 305,
    REDIR_SWITCH_PROXY = 306,
    REDIR_TEMPORARY_REDIRECT = 307,
    REDIR_RESUME_INCOMPLETE = 308,
    CERR_BAD_REQUEST = 400,
    CERR_UNAUTHORIZED = 401,
    CERR_PAYMENT_REQUIRED = 402,
    CERR_FORBIDDEN = 403,
    CERR_NOT_FOUND = 404,
    CERR_METHOD_NOT_ALLOWED = 405,
    CERR_NOT_ACCEPTABLE = 406,
    CERR_PROXY_AUTH_REQUIRED = 407,
    CERR_REQUEST_TIMEOUT = 408,
    CERR_CONFLICT = 409,
    CERR_GONE = 410,
    CERR_LEN_REQUIRED = 411,
    CERR_PRECONDITION_FAILED = 412,
    CERR_REQUEST_ENT_TOO_LARGE = 413,
    CERR_REQUEST_URI_TOO_LONG = 414,
    CERR_UNSUPPORTED_TYPE = 415,
    CERR_REQUEST_RANGE_UNSATISFIABLE = 416,
    CERR_EXPECTATION_FAILED = 417,
    CERR_TEAPOT = 418,
    CERR_UNPROCESSABLE_ENT = 422,
    CERR_LOCKED = 423,
    CERR_FAILED_DEPENDENCY = 424,
    CERR_UNORDERED_COLLECTION = 425,
    CERR_UPGRADE_REQUIRED = 426,
    CERR_NO_RESPONSE = 444,
    CERR_RETRY_WITH = 449,
    CERR_PARENTAL_CONTROLS = 450,
    CERR_CLIENT_CLOSED_REQEST = 499,
    SERR_INTERNAL_ERROR = 500,
    SERR_NOT_IMPLEMENTED = 501,
    SERR_BAD_GATEWAY = 502,
    SERR_SERVICE_UNAVAILABLE = 503,
    SERR_GATEWAY_TIMEOUT = 504,
    SERR_HTTPVERSION_NOT_SUPPORTED = 505,
    SERR_VARIABLE_ALSO_NEGOTIATES = 506,
    SERR_INSUFFICIENT_STORAGE = 507,
    SERR_BANDWIDTH_EXCEEDED = 509,
    SERR_NOT_EXTENDED = 510,
    SERR_NET_READ_TIMEOUT = 598,
    SERR_NET_CONNECT_TIMEOUT = 599
} QRestStatusCode;

typedef struct
{
    QRestMethod verb;
    QString path;
    QStringList flags;
    QMap<QString, QString> parameters;
    QByteArray payload;
    QString hostName;
    QString httpVersionTag;
    QString payloadContentType;
    unsigned short port;
} QRestRequest;

typedef struct
{
    QRestRequest request;
    QRestStatusCode returnCode;
    QByteArray payload;
    QString payloadContentType;
} QRestResponce;

class QRestRequestFactory
{
private:
    QString hostname;
    unsigned int port;
    bool sslSecure;
    QString httpVersionTag;

public:
    void setHttpVersionTag(QString tag = "HTTP/1.1")
    {
        this->httpVersionTag = tag;
    }

    QRestRequestFactory(QString hostname, unsigned short port)
    {
        this->hostname = hostname;
        this->port = port;
        this->setHttpVersionTag();
    }

    QRestRequest makeRequest(QRestMethod verb, QString path, QStringList flags = QStringList(), QMap<QString, QString> params = QMap<QString, QString>(), QByteArray payload = QByteArray())
    {
        QRestRequest req;
        req.flags = flags;
        req.hostName = this->hostname;
        req.httpVersionTag = this->httpVersionTag;
        req.parameters = params;
        req.path = path;
        req.payload = payload;
        req.payloadContentType = "application/x-www-form-urlencoded";
        req.port = this->port;
        req.verb = verb;
        return req;
    }
};

class QRestClient : public QThread
{
    Q_OBJECT
public:
    QRestClient(QRestRequest request);
    void asyncBegin();
    void run();
    QRestResponce getResponce();

signals:
    void receivedResponce(QRestClient* client);

private:
    QRestRequest req;
    QRestResponce res;
    QString urlEncode(QStringList flags, QMap<QString, QString> params);
    QString urlSafe(QString s);
};

#endif // QRESTCLIENT_H
