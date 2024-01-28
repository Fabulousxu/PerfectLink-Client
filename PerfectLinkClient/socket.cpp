#include "socket.h"

Socket::Socket(QObject *parent) {
	state = Offline;
	connect(this, &QTcpSocket::readyRead, this, &Socket::onRead);
}

void Socket::onSignupRequest(const QString &nickname, const QString &passward) {
	QJsonObject json, data;
	data.insert("nickname", nickname);
	data.insert("passward", passward);
	json.insert("request", Request::Register);
	json.insert("data", data);
	write(QJsonDocument(json).toJson());
}

void Socket::onLoginRequest(quint64 id, const QString &passward) {
	QJsonObject json, data;
	data.insert("id", QString::number(id));
	data.insert("passward", passward);
	json.insert("request", Request::Login);
	json.insert("data", data);
	write(QJsonDocument(json).toJson());

	emit loginSuccess("东川路徐先生");
}

void Socket::onLogoffRequest(quint64 id) {
	QJsonObject json, data;
	data.insert("id", QString::number(id));
	json.insert("request", Request::Logoff);
	json.insert("data", data);
	write(QJsonDocument(json).toJson());

	emit logoffSuccess();
}

void Socket::onRead() {
	auto json = QJsonDocument::fromJson(readAll()).object();
	int request = json.value("request").toInt();
	auto data = json.value("data").toObject();
	switch (request) {
		case Reply::Register: {
			auto flag = data.value("state").toBool();
			if (flag) {
				emit signupSuccess(data.value("id").toString().toULongLong());
			} else { emit signupFail(data.value("error").toString()); }
			break;
		}
		case Reply::Login: {
			auto flag = data.value("state").toBool();
			if (flag) {
				emit loginSuccess(data.value("nickname").toString());
			} else { emit loginFail(data.value("error").toString()); }
		}
		case Reply::Logoff: {
			auto flag = data.value("state").toBool();
			if (flag) {
				emit logoffSuccess();
			} else { emit logoffFail(data.value("error").toString()); }
		}
	}
}
