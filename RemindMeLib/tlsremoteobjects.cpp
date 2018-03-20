#include "tlsremoteobjects.h"

#include <QUrlQuery>
#include <QUuid>
#include <QCoreApplication>
#include <QFile>
#include <QSslKey>
#include "rotlsclientio.h"
#include "rotlsserverio.h"

namespace {

QHash<QString, QSslConfiguration> _sslConfigs;

QUrl createBaseUrl(const QString &host, quint16 port)
{
	QUrl url;
	url.setScheme(TlsRemoteObjects::UrlScheme);
	url.setHost(host);
	url.setPort(port);
	return url;
}

void __setup_tslro()
{
	qRegisterRemoteObjectsServer<RoTlsServer>(TlsRemoteObjects::UrlScheme);
	qRegisterRemoteObjectsClient<RoTlsClientIo>(TlsRemoteObjects::UrlScheme);
}

}
Q_COREAPP_STARTUP_FUNCTION(__setup_tslro)

const QString TlsRemoteObjects::UrlScheme = QStringLiteral("tls");

QUrl TlsRemoteObjects::generateP12Url(const QString &host, quint16 port, const QString &path, const QString &passPhrase)
{
	auto url = createBaseUrl(host, port);
	url.setPath(QStringLiteral("/p12"));

	QUrlQuery query;
	query.addQueryItem(QStringLiteral("path"), path);
	if(!passPhrase.isEmpty())
		query.addQueryItem(QStringLiteral("pass"), passPhrase);
	url.setQuery(query);

	return url;
}

QUrl TlsRemoteObjects::generateConfigUrl(const QString &host, quint16 port, const QSslConfiguration &config)
{
	auto url = createBaseUrl(host, port);
	url.setPath(QStringLiteral("/conf"));

	QUrlQuery query;
	query.addQueryItem(QStringLiteral("key"), prepareSslConfig(config));
	url.setQuery(query);

	return url;
}

QString TlsRemoteObjects::prepareSslConfig(const QSslConfiguration &config)
{
	auto id = QUuid::createUuid().toString();
	_sslConfigs.insert(id, config);
	return id;
}

QSslConfiguration TlsRemoteObjects::getSslConfig(const QString &key)
{
	return _sslConfigs.value(key);
}

QSslConfiguration TlsRemoteObjects::prepareFromUrl(const QUrl &url)
{
	auto type = url.path();
	QUrlQuery params(url);

	if(type == QStringLiteral("/p12")) {
		QFile file(params.queryItemValue(QStringLiteral("path")));
		if(!file.open(QIODevice::ReadOnly)) {
			qCritical() << "Failed to open file" << file.fileName() << "with error:"
						<< file.errorString();
			return {};
		}

		QByteArray passPhrase;
		if(params.hasQueryItem(QStringLiteral("pass")))
			passPhrase = params.queryItemValue(QStringLiteral("pass")).toUtf8();

		QSslKey key;
		QSslCertificate cert;
		QList<QSslCertificate> caCerts;
		if(!QSslCertificate::importPkcs12(&file, &key, &cert, &caCerts, passPhrase)) {
			qCritical() << "Failed to read" << file.fileName() << "as PKCS#12 files";
			return {};
		}
		file.close();

		auto conf = QSslConfiguration::defaultConfiguration();
		conf.setPeerVerifyMode(QSslSocket::VerifyPeer);
		conf.setCaCertificates(caCerts);
		conf.setLocalCertificate(cert);
		conf.setPrivateKey(key);
		return conf;
	} else if(type == QStringLiteral("/conf"))
		return getSslConfig(params.queryItemValue(QStringLiteral("key")));
	else {
		qCritical() << "Invalid remote objects TLS url" << url;
		return QSslConfiguration();
	}
}
