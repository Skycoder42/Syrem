#include "rotlsserverio.h"
#include "tlsremoteobjects.h"

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
	connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::error),
			this, &RoTlsServerIo::onError);
	connect(_socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
			this, &RoTlsServerIo::onSslErrors);
}

QIODevice *RoTlsServerIo::connection() const
{
	return _socket;
}

void RoTlsServerIo::onError(QAbstractSocket::SocketError error)
{
	Q_UNUSED(error)
	qWarning() << Q_FUNC_INFO << _socket->errorString();
}

void RoTlsServerIo::onSslErrors(const QList<QSslError> &errors)
{
	for(auto error : errors)
		qWarning() << Q_FUNC_INFO << error.errorString();
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
	connect(_server, QOverload<QAbstractSocket::SocketError>::of(&QSslServer::acceptError),
			this, &RoTlsServer::onAcceptError);
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

	auto conf = TlsRemoteObjects::prepareFromUrl(address);
	if(conf.isNull())
		return false;

	_server->setSslConfiguration(conf);
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

void RoTlsServer::onAcceptError(QAbstractSocket::SocketError socketError)
{
	Q_UNUSED(socketError)
	qWarning() << Q_FUNC_INFO << _server->errorString();
}
