#include "socket.h"

Socket::Socket(QObject *parent)
	: QTcpSocket(parent)
{
	connect(this, &QTcpSocket::readyRead, this, &Socket::onRead);
}

void Socket::request(Request::Type request, const QJsonObject &data)
{
	QJsonObject json;
	json.insert("request", request);
	json.insert("data", data);
	write(QJsonDocument(json).toJson());
	flush();
}

void Socket::onSignupRequest(const QString &nickname, const QString &password) {
	QJsonObject data;
	data.insert("nickname", nickname);
	data.insert("password", password);
	request(Request::Signup, data);

	/* 以下测试用 */
	//emit signupSuccess(10001);
}

void Socket::onLoginRequest(quint64 id, const QString &password) {
	QJsonObject data;
	data.insert("id", QString::number(id));
	data.insert("password", password);
	request(Request::Login, data);

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
	QJsonObject data;
	data.insert("playerLimit", playerNumber);
	data.insert("height", h);
	data.insert("width", w);
	data.insert("patternNumber", patternNumber);
	data.insert("time", time);
	request(Request::CreateRoom, data);

	/* 以下测试用 */
	//emit createRoom(10001);
}

void Socket::onRequireRoomRequest(int playerNumber)
{
	QJsonObject data;
	data.insert("playerLimit", playerNumber);
	request(Request::RequireRoom, data);

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
	QJsonObject data;
	data.insert("roomId", QString::number(rid));
	request(Request::EnterRoom, data);

}

void Socket::onRead() {
	auto json = QJsonDocument::fromJson(readAll()).object();

	qDebug() << json;

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
			QVector<QPair<quint64, int>> roomInfomation;
			for (auto jsonValue : data.value("roomInfo").toArray()) {
				auto room = jsonValue.toObject();
				auto roomId = room.value("id").toString().toULongLong();
				auto playerCount = room.value("playerCount").toInt();
				roomInfomation.append(QPair<quint64, int>(roomId, playerCount));
			}
			emit requireRoom(roomInfomation);
		} break;
		case Reply::EnterRoom: {
			auto flag = data.value("state").toBool();
			if (flag) {
				auto width = data.value("width").toInt();
				auto height = data.value("height").toInt();
				auto playerLimit = data.value("playerLimit").toInt();
				auto patternNumber = data.value("patternNumber").toInt();
				auto time = data.value("time").toInt();
				QVector<QPair<quint64, QString>> playerInfomation;
				for (auto jsonValue : data.value("playerInfo").toArray()) {
					auto player = jsonValue.toObject();
					auto id = player.value("id").toString().toULongLong();
					auto nickname = player.value("nickname").toString();
					playerInfomation.append(QPair<quint64, QString>(id, nickname));
				}
				emit enterRoomSuccess(playerLimit, width, height, patternNumber, time, playerInfomation);
			} else { 
				auto error = data.value("error").toString();
				emit enterRoomFail(error);
			}
		} break;
	}
}
