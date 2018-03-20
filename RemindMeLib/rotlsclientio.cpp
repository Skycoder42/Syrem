#include "rotlsclientio.h"
#include "tlsremoteobjects.h"

#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>

RoTlsClientIo::RoTlsClientIo(QObject *parent) :
	ClientIoDevice(parent),
	_socket(new QSslSocket(this))
{
	connect(_socket, &QSslSocket::readyRead,
			this, &RoTlsClientIo::readyRead);
	connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::error),
			this, &RoTlsClientIo::onError);
	connect(_socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
			this, &RoTlsClientIo::onSslErrors);
	connect(_socket, &QSslSocket::stateChanged,
			this, &RoTlsClientIo::onStateChanged);
}

RoTlsClientIo::~RoTlsClientIo()
{
	close();
}

QIODevice *RoTlsClientIo::connection()
{
	return _socket;
}

void RoTlsClientIo::connectToServer()
{
	if (isOpen())
		return;

	QHostAddress address(url().host());
	if(address.isNull()) {
		const auto addresses = QHostInfo::fromName(url().host()).addresses();
		if(addresses.isEmpty())
			return;
		address = addresses.first();
	}

	auto conf = TlsRemoteObjects::prepareFromUrl(url());
	if(conf.isNull())
		return;

	_socket->setSslConfiguration(conf);
	_socket->connectToHostEncrypted(address.toString(), url().port());
}

bool RoTlsClientIo::isOpen()
{
	return (!isClosing() && (_socket->state() == QAbstractSocket::ConnectedState
							 || _socket->state() == QAbstractSocket::ConnectingState));
}

void RoTlsClientIo::onError(QAbstractSocket::SocketError error)
{
	qWarning() << Q_FUNC_INFO << _socket->errorString();

	switch (error) {
	case QAbstractSocket::HostNotFoundError:     //Host not there, wait and try again
	case QAbstractSocket::ConnectionRefusedError:
		emit shouldReconnect(this);
		break;
	default:
		break;
	}
}

void RoTlsClientIo::onSslErrors(const QList<QSslError> &errors)
{
	for(auto error : errors)
		qWarning() << Q_FUNC_INFO << error.errorString();
}

void RoTlsClientIo::onStateChanged(QAbstractSocket::SocketState state)
{
	if (state == QAbstractSocket::ClosingState && !isClosing()) {
		_socket->abort();
		emit shouldReconnect(this);
	}
	if (state == QAbstractSocket::ConnectedState) {
		m_dataStream.setDevice(connection());
		m_dataStream.resetStatus();
	}
}

void RoTlsClientIo::doClose()
{
	if(_socket->isOpen()) {
		connect(_socket, &QSslSocket::disconnected,
				this, &QObject::deleteLater);
		_socket->disconnectFromHost();
	} else
		deleteLater();
}
