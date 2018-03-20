#ifndef ROTLSSERVERIO_H
#define ROTLSSERVERIO_H

#include <QObject>
#include <QSslSocket>
#include <qsslserver.h>
#include <QtRemoteObjects/QConnectionAbstractServer>

class RoTlsServerIo : public ServerIoDevice
{
	Q_OBJECT
public:
	explicit RoTlsServerIo(QSslSocket *socket, QObject *parent = nullptr);

	QIODevice *connection() const override;

protected:
	void doClose() override;

private:
	QSslSocket *_socket;
};

class RoTlsServer : public QConnectionAbstractServer
{
	Q_OBJECT
	Q_DISABLE_COPY(RoTlsServer)

public:
	explicit RoTlsServer(QObject *parent);
	~RoTlsServer();

	bool hasPendingConnections() const override;
	ServerIoDevice *configureNewConnection() override;
	QUrl address() const override;
	bool listen(const QUrl &address) override;
	QAbstractSocket::SocketError serverError() const override;
	void close() override;

private Q_SLOTS:
	void onAcceptError(QAbstractSocket::SocketError socketError);
	void onError(QAbstractSocket::SocketError error);
	void onSslErrors(const QList<QSslError> &errors);

private:
	QSslServer *_server;
	QUrl _originalUrl;
};

#endif // ROTLSSERVERIO_H
