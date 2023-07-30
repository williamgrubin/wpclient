#ifndef WIDGET_H
#define WIDGET_H

// Protocol Macros
#define MESSAGE 1
#define CHANGEROOM 2
#define FIGHT 3
#define PVP 4
#define LOOT 5
#define START 6
#define ERROR 7
#define ACCEPT 8
#define ROOM 9
#define CHARACTER 10
#define GAME 11
#define LEAVE 12
#define CONNECTION 13
#define VERSION 14

// C
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <bits/stdc++.h>

// C++
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include <algorithm>

// Qt
#include <QWidget>
#include <QDebug>
#include <QCloseEvent>
#include <QApplication>
#include <QObject>
#include <QThread>
#include <QString>
#include <QMetaType>
#include <QTextBlock>
#include <QMetaObject>
#include <QMutex>
#include <QScrollBar>
#include <QKeyEvent>
#include "ui_widget.h"

// Miscellaneous
#include "protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    uint sfd;

    int health;
    uint16_t initial_points, stat_limit;
    uint16_t destination_room;
    uint attack, defense, regen, gold, room;
    bool auto_pve;

    QString name, description, target, message_recipient, message;
    QString listen_address, listen_port;
    QString QError, QMessage, QConnection, QRoom, QCharacter, QAccept, QVersion, QGame;

    map<string, QString> neighbor_map;

    Ui::Widget *ui;

    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:

signals:
    void change_main_text(QString arg1);

    void change_neighbor_board(QString arg1);

    void reset_neighbor_board();

    void change_message_board(QString arg1);

private slots:
    void keyPressEvent(QKeyEvent *event);

    void on_characterButton_clicked();

    void on_nameText_textChanged();

    void on_joinBattleCheckBox_stateChanged(int arg1);

    void on_attackSpinBox_valueChanged(int arg1);

    void on_defenseSpinBox_valueChanged(int arg1);

    void on_regenSpinBox_valueChanged(int arg1);

    void on_descriptionText_textChanged();

    void on_messagePromptButton_clicked();

    void on_leaveButton_clicked();

    void on_messageText_textChanged();

    void on_messageRecipientText_textChanged();

    void on_fightButton_clicked();

    void on_pvpButton_clicked();

    void on_lootButton_clicked();

    void on_startButton_clicked();

    void on_changeroomButton_clicked();

    void on_healthSpinBox_valueChanged(int arg1);

    void on_roomSpinBox_valueChanged(int arg1);

    void on_goldSpinBox_valueChanged(int arg1);

    void on_addressText_textChanged();

    void on_portSpinBox_valueChanged(int arg1);

    void on_connectButton_clicked();

    void on_changeroomSpinBox_valueChanged(int arg1);

    void on_targetText_textChanged();

    void on_cancelTargetButton_clicked();

    void on_cancelMessageButton_clicked();

private:
};
#endif // WIDGET_H
