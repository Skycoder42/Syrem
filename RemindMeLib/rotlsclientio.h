#ifndef ROTLSCLIENTIO_H
#define ROTLSCLIENTIO_H

#include <QObject>
#include <QSslSocket>
#include <QtRemoteObjects/QConnectionAbstractServer>

class RoTlsClientIo : public ClientIoDevice
{
	Q_OBJECT

public:
	explicit RoTlsClientIo(QObject *parent = nullptr);
	~RoTlsClientIo() override;

	QIODevice *connection() override;
	void connectToServer() override;
	bool isOpen() override;

public Q_SLOTS:
	void onError(QAbstractSocket::SocketError error);
	void onSslErrors(const QList<QSslError> &errors);
	void onStateChanged(QAbstractSocket::SocketState state);

protected:
	void doClose() override;

private:
	QSslSocket *_socket;
};

#endif // ROTLSCLIENTIO_H
