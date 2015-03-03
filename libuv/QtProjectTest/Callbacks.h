#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <memory>
#include <functional>
#include <QObject>

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::weak_ptr<TcpConnection> TcpConnectionWeakPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&, const QByteArray&)> MessageCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;

#endif // CALLBACKS_H
