#include "qrestclient.h"

QRestClient::QRestClient(QRestRequest request) :
    QThread()
{
    this->req = request;
}

void QRestClient::asyncBegin()
{
    this->start();
}

QRestResponce QRestClient::getResponce()
{
    return this->res;
}

QString QRestClient::urlSafe(QString s)
{
    return s.replace(" ", "+");
}

QString QRestClient::urlEncode(QStringList flags, QMap<QString, QString> params)
{
    QStringList parts;
    foreach (QString flag, flags)
    {
        parts << flag;
    }
    foreach (QString key, params.keys())
    {
        parts << (key.append("=").append(params[key]));
    }
    return parts.join("&");
}

void QRestClient::run()
{
    QSslSocket ssl;
    ssl.setPeerVerifyMode(QSslSocket::QueryPeer);
    ssl.connectToHostEncrypted(req.hostName, req.port);

    res.request = req;

    if (ssl.waitForConnected())
    {
        QStringList builder;
        QString params = urlEncode(req.flags, req.parameters);
        switch (req.verb)
        {
        case POST:
            builder << "POST ";
            builder << req.path;
            builder << "\n";
            builder << req.hostName.prepend("Host: ");
            builder << "\n";
            builder << "Content-Type: application/x-www-form-urlencoded";
            builder << "\n";
            builder << QString::number(params.toAscii().size()).prepend("Content-Length: ");
            builder << "\n\n";
            builder << params.toAscii();
            break;
        case GET:
            builder << "GET ";
            builder << req.path.append(urlEncode(req.flags, req.parameters).prepend("?"));
            builder << "\n";
            builder << req.hostName.prepend("Host: ");
            break;
        case PUT:
            builder << "PUT ";
            builder << req.path.append(urlEncode(req.flags, req.parameters).prepend("?"));
            builder << "\n";
            builder << req.hostName.prepend("Host: ");
            builder << "\n";
            builder << req.payloadContentType.prepend("Content-Type: ");
            builder << "\n";
            builder << QString::number(req.payload.size()).prepend("Content-Length: ");
            builder << "\n\n";
            break;
        case DELETE:
            builder << "DELETE ";
            builder << req.path.append(urlEncode(req.flags, req.parameters).prepend("?"));
            builder << "\n";
            builder << req.hostName.prepend("Host: ");
            break;
        default:
            ssl.close();
            res.returnCode = CERR_UNPROCESSABLE_ENT;
            res.payload.clear();
            this->receivedResponce(this);
            return;
        }

        QString data = builder.join("");
        QByteArray buffer;
        buffer.append(data);
        ssl.write(buffer);
        if (ssl.waitForReadyRead())
        {
            res.returnCode = (QRestStatusCode) ssl.readLine().split(' ').at(1).toInt();

            for (;;)
            {
                QByteArray d = ssl.readLine();
                QString line(d);
                if (d.isEmpty() || line.trimmed() == "")
                {
                    break;
                }

                QStringList pieces = line.split(' ');
                if (pieces.first() == "Content-Type:")
                {
                    res.payloadContentType = pieces.last();
                }
            }

            res.payload = ssl.readAll();
        }
        else
        {
            res.returnCode = SERR_NET_READ_TIMEOUT;
            res.payload.clear();
        }
    }
    else
    {
        res.returnCode = SERR_NET_CONNECT_TIMEOUT;
        res.payload.clear();
    }
    ssl.close();
    this->receivedResponce(this);
}

