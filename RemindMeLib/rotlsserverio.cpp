#include "rotlsserverio.h"

#include <QHostInfo>

RoTlsServerIo::RoTlsServerIo(QSslSocket *socket, QObject *parent) :
	ServerIoDevice(parent),
	_socket(socket)
{
	_socket->setParent(this);
	connect(_socket, &QSslSocket::readyRead,
			this, &RoTlsServerIo::readyRead);
	connect(_socket, &QSslSocket::disconnected,
			this, &RoTlsServerIo::disconnected);
}

QIODevice *RoTlsServerIo::connection() const
{
	return _socket;
}

void RoTlsServerIo::doClose()
{
	_socket->disconnectFromHost();
}



RoTlsServer::RoTlsServer(QObject *parent) :
	QConnectionAbstractServer(parent),
	_server(new QSslServer(this)),
	_originalUrl()
{
	connect(_server, &QSslServer::newConnection,
			this, &RoTlsServer::newConnection);
}

RoTlsServer::~RoTlsServer()
{
	close();
}

bool RoTlsServer::hasPendingConnections() const
{
	return _server->hasPendingConnections();
}

ServerIoDevice *RoTlsServer::configureNewConnection()
{
	if(!_server->isListening() || !_server->hasPendingConnections())
		return nullptr;
	else
		return new RoTlsServerIo(_server->nextPendingSslConnection());
}

QUrl RoTlsServer::address() const
{
	return _originalUrl;
}

bool RoTlsServer::listen(const QUrl &address)
{
	QHostAddress host(address.host());
	if (host.isNull()) {
		if(address.host().isEmpty()) {
			host = QHostAddress::Any;
		} else {
			qWarning() << address.host() << " is not an IP address, trying to resolve it";
			auto info = QHostInfo::fromName(address.host());
			if (info.addresses().isEmpty())
				host = QHostAddress::Any;
			else
				host = info.addresses().constFirst();
		}
	}

	if(_server->listen(host, address.port())) {
		_originalUrl = address;
		return true;
	} else
		return false;
}

QAbstractSocket::SocketError RoTlsServer::serverError() const
{
	//cannot return ssl errors for now
	return _server->serverError();
}

void RoTlsServer::close()
{
	_server->close();
}
