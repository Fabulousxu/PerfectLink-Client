#include "socket.h"

#define TEST 0

bool isLittleEndian() {
	int num = 0x1;
	return *(char *)(&num) == 0x1;
}

bool isBigEndian() { return !isLittleEndian(); }
quint32 fromBigEndian(const QByteArray &numBytes) {
	quint32 numInt;
	auto numChar = numBytes.data();
	if (isBigEndian()) {
		numInt = *(int *)(numChar);
	} else {
		*(char *)&numInt = numChar[3];
		*((char *)&numInt + 1) = numChar[2];
		*((char *)&numInt + 2) = numChar[1];
		*((char *)&numInt + 3) = numChar[0];
	}
	return numInt;
}

QByteArray toBigEndian(quint32 num) {
	char numChar[5];
	if (isBigEndian()) {
		*(int *)numChar = num;
	} else {
		numChar[0] = *((char *)&num + 3);
		numChar[1] = *((char *)&num + 2);
		numChar[2] = *((char *)&num + 1);
		numChar[3] = *(char *)&num;
	}
	numChar[4] = '\0';
	return QByteArray(numChar, 4);
}


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
	QByteArray bytes = QJsonDocument(json).toJson();
	quint32 length = bytes.length();
	write(toBigEndian(length) + bytes);
	//flush();
}

void Socket::onSignupRequest(const QString &nickname, const QString &password) {
	QJsonObject data;
	data.insert("nickname", nickname);
	data.insert("password", password);
	request(Request::Signup, data);

#if TEST
	emit signupSuccess(1001);
#endif
}

void Socket::onLoginRequest(quint64 id, const QString &password) {
	QJsonObject data;
	data.insert("id", QString::number(id));
	data.insert("password", password);
	request(Request::Login, data);

#if TEST
	emit loginSuccess("用户1001");
#endif
}

void Socket::onLogoffRequest(quint64 id) {
	//QJsonObject json, data;
	//data.insert("id", QString::number(id));
	//json.insert("request", Request::Logoff);
	//json.insert("data", data);
	//write(QJsonDocument(json).toJson());
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

#if TEST
	emit createRoom(1234);
#endif
}

void Socket::onRequireRoomRequest(int playerNumber)
{
	QJsonObject data;
	data.insert("playerLimit", playerNumber);
	request(Request::RequireRoom, data);

#if TEST
	//QVector<QPair<quint64, int>> vec;
	//vec.append(QPair<quint64, int>(1, 1));
	//vec.append(QPair<quint64, int>(2, 1));
	//vec.append(QPair<quint64, int>(3, 1));
#endif
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

void Socket::onExitRoomRequest()
{
	QJsonObject data;
	request(Request::ExitRoom, data);
}

void Socket::onPrepareRequest(bool prepare) {
	QJsonObject data;
	data.insert("prepare", prepare);
	request(Request::Prepare, data);
}

void Socket::onMoveRequest(Direction direction)
{
	QJsonObject data;
	data.insert("direction", direction);
	request(Request::Move, data);

#if TEST
	emit test(direction);
#endif
}

void Socket::onRead() {
	while (bytesAvailable()) {
		quint32 length = fromBigEndian(read(4)); //TODO 大小端
		QByteArray bytes = read(length);
		auto json = QJsonDocument::fromJson(bytes).object();

		qDebug() << "receive: [[\n\t" << json << "\n]]";

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
					auto roomId = room.value("roomId").toString().toULongLong();
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
					QVector<QPair<quint64, QPair<QString, bool>>> playerInfomation;
					for (auto jsonValue : data.value("playerInfo").toArray()) {
						auto player = jsonValue.toObject();
						auto id = player.value("id").toString().toULongLong();
						auto nickname = player.value("nickname").toString();
						auto prepare = player.value("prepare").toBool();
						playerInfomation.append(
							QPair<quint64, QPair<QString, bool>>(id, QPair<QString, bool>(nickname, prepare))
						);
					}
					emit enterRoomSuccess(playerLimit, width, height, patternNumber, time, playerInfomation);
				} else {
					auto error = data.value("error").toString();
					emit enterRoomFail(error);
				}
			} break;
			case Reply::ExitRoom: {
				auto flag = data.value("state").toBool();
				if (flag) {
					emit exitRoomSuccess();
				} else { emit exitRoomFail(data.value("error").toString()); }
			} break;
			case Reply::Prepare: {
				auto id = data.value("playerId").toString().toULongLong();
				auto prepare = data.value("prepare").toBool();
				emit playerPrepare(id, prepare);
			} break;
			case Reply::PlayerChange: {
				auto enter = data.value("enter").toBool();
				auto id = data.value("playerId").toString().toULongLong();
				if (enter) {
					emit playerEnter(id, data.value("nickname").toString());
				} else { emit playerExit(id); }
			} break;
			case Reply::Begin: {
				QVector<QVector<int>> map;
				QVector<QPair<quint64, QPoint>> playerPosition;
				for (auto xValue : data.value("map").toArray()) {
					QVector<int> xMap;
					for (auto yValue : xValue.toArray()) { xMap.append(yValue.toInt()); }
					map.append(xMap);
				}
				for (auto jsonValue : data.value("playerPos").toArray()) {
					auto player = jsonValue.toObject();
					auto x = player.value("x").toInt();
					auto y = player.value("y").toInt();
					auto id = player.value("playerId").toString().toULongLong();
					playerPosition.append(QPair<quint64, QPoint>(id, QPoint(x, y)));
				}
				emit gameBegin(map, playerPosition);
			} break;
			case Reply::Move: {
				auto playerId = data.value("playerId").toString().toULongLong();
				auto direction = (Direction)data.value("direction").toInt();
				auto state = data.value("state").toBool();
				emit playerMove(playerId, direction, state);
			} break;
			case Reply::Select: {
				auto flag = data.value("state").toBool();
				auto x = data.value("x").toInt();
				auto y = data.value("y").toInt();
				if (flag) {
					auto id = data.value("playerId").toString().toULongLong();
					emit selectBlock(QPoint(x, y), id);
				} else { emit unSelectBlock(QPoint(x, y)); }
			} break;
			case Reply::Path: {
				QVector<QPoint> path;
				for (auto jsonValue : data.value("path").toArray()) {
					auto p = jsonValue.toObject();
					auto x = p.value("x").toInt();
					auto y = p.value("y").toInt();
					path.append(QPoint(x, y));
				}
				emit drawPath(path);
			} break;
			case Reply::Mark: {
				auto id = data.value("playerId").toString().toULongLong();
				auto score = data.value("score").toInt();
				emit mark(id, score);
			} break;
			case Reply::End: {
				QVector<QPair<quint64, int>> rank;
				for (auto jsonValue : data.value("rank").toArray()) {
					auto p = jsonValue.toObject();
					auto playerId = p.value("playerId").toString().toULongLong();
					auto score = p.value("score").toInt();
					rank.append(QPair<quint64, int>(playerId, score));
				}
				emit gameEnd(rank);
			} break;
		}
	}
}
