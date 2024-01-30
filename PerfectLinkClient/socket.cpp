#include "socket.h"

Socket::Socket(QObject *parent)
	: QTcpSocket(parent)
{
	connect(this, &QTcpSocket::readyRead, this, &Socket::onRead);
}

void Socket::onSignupRequest(const QString &nickname, const QString &password) {
	QJsonObject json, data;
	data.insert("nickname", nickname);
	data.insert("password", password);
	json.insert("request", Request::Signup);
	json.insert("data", data);
	write(QJsonDocument(json).toJson());
}

void Socket::onLoginRequest(quint64 id, const QString &password) {
	QJsonObject json, data;
	data.insert("id", QString::number(id));
	data.insert("password", password);
	json.insert("request", Request::Login);
	json.insert("data", data);
	write(QJsonDocument(json).toJson());
}

void Socket::onLogoffRequest(quint64 id) {
	QJsonObject json, data;
	data.insert("id", QString::number(id));
	json.insert("request", Request::Logoff);
	json.insert("data", data);
	write(QJsonDocument(json).toJson());
}

void Socket::onRead() {
	auto json = QJsonDocument::fromJson(readAll()).object();
	Reply::Type reply = (Reply::Type)json.value("reply").toInt();
	auto data = json.value("data").toObject();
	switch (reply) {
		case Reply::Signup: {
			auto flag = data.value("state").toBool();
			if (flag) {
				emit signupSuccess(data.value("id").toString().toULongLong());
			} else { emit signupFail(data.value("error").toString()); }
		} break;
		case Reply::Login: {
			auto flag = data.value("state").toBool();
			if (flag) {
				emit loginSuccess(data.value("nickname").toString());
			} else { emit loginFail(data.value("error").toString()); }
		} break;
		case Reply::Logoff: {
			auto flag = data.value("state").toBool();
			if (flag) {
				emit logoffSuccess();
			} else { emit logoffFail(data.value("error").toString()); }
		}
	}
}
