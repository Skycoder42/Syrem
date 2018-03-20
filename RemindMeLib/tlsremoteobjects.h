#ifndef TLSREMOTEOBJECTS_H
#define TLSREMOTEOBJECTS_H

#include <QSslConfiguration>
#include <QString>
#include <QUrl>

namespace TlsRemoteObjects
{

extern const QString UrlScheme;

// tls://<host>:<port>/p12?path=<path>&pass=<passPhrase>
QUrl generateP12Url(const QString &host, quint16 port,
					const QString &path,
					const QString &passPhrase = QString());
// tls://<host>:<port>/conf?key=<config>
QUrl generateConfigUrl(const QString &host, quint16 port,
					   const QSslConfiguration &config);

QSslConfiguration prepareFromUrl(const QUrl &url);

QString prepareSslConfig(const QSslConfiguration &config);
QSslConfiguration getSslConfig(const QString &key);

}

#endif // TLSREMOTEOBJECTS_H
