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

	/* 以下测试用 */
	//emit signupSuccess(10001);
}

void Socket::onLoginRequest(quint64 id, const QString &password) {
	QJsonObject json, data;
	data.insert("id", QString::number(id));
	data.insert("password", password);
	json.insert("request", Request::Login);
	json.insert("data", data);
	write(QJsonDocument(json).toJson());

	/* 以下测试用 */
	//emit loginSuccess("用户10001");
}

void Socket::onLogoffRequest(quint64 id) {
	//QJsonObject json, data;
	//data.insert("id", QString::number(id));
	//json.insert("request", Request::Logoff);
	//json.insert("data", data);
	//write(QJsonDocument(json).toJson());

	/* 以下测试用 */
	//emit logoffSuccess();
}

void Socket::onCreateRoomRequest(int playerNumber, int w, int h, int patternNumber, int time)
{
	QJsonObject json, data;
	data.insert("playerNumber", playerNumber);
	data.insert("height", h);
	data.insert("width", w);
	data.insert("patternNumber", patternNumber);
	data.insert("time", time);
	json.insert("request", Request::CreateRoom);
	json.insert("data", data);
	write(QJsonDocument(json).toJson());

	/* 以下测试用 */
	//emit createRoom(10001);
}

void Socket::onRequireRoomRequest(int playerNumber)
{
	QJsonObject json, data;
	data.insert("playerLimit", playerNumber);
	json.insert("request", Request::RequireRoom);
	json.insert("data", data);
	write(QJsonDocument(json).toJson());

	/* 以下测试用 */
	//QVector<QPair<quint64, int>> vec;
	//vec.append(QPair<quint64, int>(1, 1));
	//vec.append(QPair<quint64, int>(2, 1));
	//vec.append(QPair<quint64, int>(3, 1));
	//emit requireRoom(vec);
}

void Socket::onSearchRoomRequest(quint64 rid)
{
}

void Socket::onEnterRoomRequest(quint64 rid)
{
	QJsonObject json, data;
	data.insert("roomId", QString::number(rid));
	json.insert("request", Request::EnterRoom);
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
		//case Reply::Logoff: {
		//	auto flag = data.value("state").toBool();
		//	if (flag) {
		//		emit logoffSuccess();
		//	} else { emit logoffFail(data.value("error").toString()); }
		//} break;
		case Reply::CreateRoom: {
			emit createRoom(data.value("roomId").toString().toULongLong());
		} break;
		case Reply::RequireRoom: {

		} break;
	}
}
