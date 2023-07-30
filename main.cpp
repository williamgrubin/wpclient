#include "widget.h"

Q_DECLARE_METATYPE(QTextBlock);
Q_DECLARE_METATYPE(QTextCursor);

QMutex emitter_mutex;
QMutex protocol_mutex;

void handle_error(int sfd, Widget* our_widget) {
    class lurk_error* our_error = new lurk_error();

    string stdError;
    QString QError;

    recv(sfd, our_error, 4, MSG_WAITALL);
    our_error->error_message = (char*)calloc(our_error->error_length + 1, sizeof(char));
    recv(sfd, our_error->error_message, our_error->error_length, MSG_WAITALL);

    stdError = "[ERROR]: " + (string)our_error->error_message;
    QError = QString::fromStdString(stdError);
    our_widget->QError = QError;

    emitter_mutex.lock();
    emit our_widget->change_main_text(QError);
    emitter_mutex.unlock();

    if(our_error->error_code == 2 || our_error->error_code == 4) {
        our_widget->ui->characterButton->setVisible(true);
        our_widget->ui->characterButton->setEnabled(true);
        our_widget->ui->nameText->setReadOnly(false);
        our_widget->ui->descriptionText->setReadOnly(false);
        our_widget->ui->healthSpinBox->setReadOnly(false);
        our_widget->ui->regenSpinBox->setReadOnly(false);
        our_widget->ui->attackSpinBox->setReadOnly(false);
        our_widget->ui->defenseSpinBox->setReadOnly(false);
        our_widget->ui->roomSpinBox->setReadOnly(false);
        our_widget->ui->goldSpinBox->setReadOnly(false);
        our_widget->ui->joinBattleCheckBox->setEnabled(true);
        our_widget->ui->startButton->setVisible(false);
        our_widget->ui->startButton->setEnabled(false);
        our_widget->ui->nameText->setFocusPolicy(Qt::StrongFocus);
        our_widget->ui->descriptionText->setFocusPolicy(Qt::StrongFocus);
        our_widget->ui->healthSpinBox->setFocusPolicy(Qt::StrongFocus);
        our_widget->ui->regenSpinBox->setFocusPolicy(Qt::StrongFocus);
        our_widget->ui->attackSpinBox->setFocusPolicy(Qt::StrongFocus);
        our_widget->ui->defenseSpinBox->setFocusPolicy(Qt::StrongFocus);
        our_widget->ui->roomSpinBox->setFocusPolicy(Qt::StrongFocus);
        our_widget->ui->goldSpinBox->setFocusPolicy(Qt::StrongFocus);
        our_widget->ui->nameText->setFocus();
    }

    free(our_error->error_message);
    delete our_error;
    return;
}

void handle_incoming_message(int sfd, Widget* our_widget) {
    class lurk_message* inbound_message = new lurk_message();

    string stdMessage;
    QString QMessage;

    recv(sfd, inbound_message, 67, MSG_WAITALL);
    inbound_message->message = (char*)calloc(inbound_message->message_length + 1, sizeof(char));
    recv(sfd, inbound_message->message, inbound_message->message_length, MSG_WAITALL);

    if(inbound_message->narration_marker[0] == 0 && inbound_message->narration_marker[1] == 1) { stdMessage = (string)inbound_message->sender_name + " (narrator): " + inbound_message->message; }
    else { stdMessage = (string)inbound_message->sender_name + ": " + inbound_message->message; }
    QMessage = QString::fromStdString(stdMessage);
    our_widget->QMessage = QMessage;

    emitter_mutex.lock();
    emit our_widget->change_message_board(QMessage);
    emitter_mutex.unlock();

    free(inbound_message->message);
    delete inbound_message;
    return;
}

void handle_connection(int sfd, Widget* our_widget) {
    class lurk_room* connection = new lurk_room();

    string stdConnection;
    QString QConnection;

    recv(sfd, connection, 37, MSG_WAITALL);
    connection->description = (char*)calloc(connection->description_length + 1, sizeof(char));
    recv(sfd, connection->description, connection->description_length, MSG_WAITALL);

    stdConnection = "[CONNECTION]: " + (string)connection->room_name + " (" + to_string(connection->room_number) + ")";
    QConnection = QString::fromStdString(stdConnection);
    our_widget->QConnection = QConnection;

    emitter_mutex.lock();
    emit our_widget->change_main_text(QConnection);
    emitter_mutex.unlock();

    free(connection->description);
    delete connection;
    return;
}

void handle_room(int sfd, Widget* our_widget) {
    class lurk_room* our_room = new lurk_room();

    string stdRoom;
    QString QRoom;

    recv(sfd, our_room, 37, MSG_WAITALL);

    our_room->description = (char*)calloc(our_room->description_length + 1, sizeof(char));
    recv(sfd, our_room->description, our_room->description_length, MSG_WAITALL);

    stdRoom = "You have entered room " + to_string(our_room->room_number) + ":\n" + our_room->room_name + "\n" + our_room->description;
    QRoom = QString::fromStdString(stdRoom);
    our_widget->QRoom = QRoom;

    emitter_mutex.lock();
    emit our_widget->change_main_text(QRoom);
    emitter_mutex.unlock();

    free(our_room->description);
    delete our_room;
    return;
}

void handle_character(int sfd, Widget* our_widget) {
    class lurk_character* character = new lurk_character;

    string stdCharacter, stdStatus;
    QString QCharacter;

    recv(sfd, character, 48, MSG_WAITALL);
    character->description = (char*)calloc(character->description_length + 1, sizeof(char));
    recv(sfd, character->description, character->description_length, MSG_WAITALL);

    stdStatus = (character->alive == true)? " ✔ " : " ❌ ";

    string stdName(character->player_name);

    if(!strcmp(our_widget->ui->nameText->toPlainText().toStdString().c_str(), character->player_name)) {
        our_widget->health = character->health_points;
        our_widget->gold = character->gold_count;
        our_widget->room = character->current_room;

        our_widget->ui->healthSpinBox->setValue(our_widget->health);
        our_widget->ui->goldSpinBox->setValue(our_widget->gold);
        our_widget->ui->roomSpinBox->setValue(our_widget->room);

        free(character->description);
        delete character;
        return;
    }

    if(character->health_points <= our_widget->stat_limit) {
        if(character->monster == 1) {
            stdCharacter =  "[MONSTER]: " + (string)character->player_name + " 💗 " + to_string(character->health_points) +
                    " 🛡️ " + to_string(character->defense_rating) + " 💞 " + to_string(character->regeneration) +
                    " 💰 " + to_string(character->gold_count) + stdStatus;
        }
        else {
            stdCharacter = (string)character->player_name + " 💗 " + to_string(character->health_points) +
                    " 🛡️ " + to_string(character->defense_rating) + " 💞 " + to_string(character->regeneration) +
                    " 💰 " + to_string(character->gold_count) + stdStatus;
        }
    } else if(character->health_points > our_widget->stat_limit) {
        if(character->monster == 1) {
            stdCharacter =  " [MONSTER]: " + (string)character->player_name + " 💗 " + to_string((int)character->health_points - 65535) +
                    " 🛡️ " + to_string(character->defense_rating) + " 💞 " + to_string(character->regeneration) +
                    " 💰 " + to_string(character->gold_count) + stdStatus;
        }
        else {
            stdCharacter = (string)character->player_name + " 💗 " + to_string((int)character->health_points - 65535) +
                    " 🛡️ " + to_string(character->defense_rating) + " 💞 " + to_string(character->regeneration) +
                    " 💰 " + to_string(character->gold_count) + stdStatus;
        }
    }

    QCharacter = QString::fromStdString(stdCharacter);
    our_widget->QCharacter = QCharacter;

    if(character->monster == 1) {
        emitter_mutex.lock();
        emit our_widget->change_main_text(QCharacter);
        emitter_mutex.unlock();

        free(character->description);
        delete character;
        return;
    } else if(character->monster == 0) {
        our_widget->neighbor_map.insert(make_pair(stdName, QCharacter));

        for(auto i : our_widget->neighbor_map) {
            if(!strcmp(stdName.c_str(), i.first.c_str())) {
                if(character->current_room != our_widget->ui->roomSpinBox->value()) { our_widget->neighbor_map.erase(i.first); }

                if(character->current_room == our_widget->ui->roomSpinBox->value()) {
                    our_widget->neighbor_map.erase(i.first);
                    our_widget->neighbor_map.insert(make_pair(stdName, QCharacter));
                }

                emitter_mutex.lock();
                emit our_widget->reset_neighbor_board();
                emitter_mutex.unlock();

                for(auto j : our_widget->neighbor_map) {
                    emitter_mutex.lock();
                    emit our_widget->change_neighbor_board(j.second);
                    emitter_mutex.unlock();
                }

                free(character->description);
                delete character;
                return;
            }
        }

        emitter_mutex.lock();
        emit our_widget->change_neighbor_board(QCharacter);
        emitter_mutex.unlock();

        free(character->description);
        delete character;
        return;
    }
}

void handle_accept(int sfd, Widget* our_widget) {
    class lurk_accept* our_accept = new lurk_accept();
    string stdAccept = "[ACCEPT]: ";
    QString QAccept;

    recv(sfd, our_accept, 2, MSG_WAITALL);

    switch (our_accept->accepted_type) {
    case MESSAGE :
        stdAccept += "Successfully messaged another player.";
        break;
    case CHANGEROOM :
        stdAccept += "Successfully changed rooms.";
        our_widget->ui->roomSpinBox->setValue(our_widget->destination_room);
        our_widget->ui->changeroomSpinBox->setFocus();
        break;
    case FIGHT :
        stdAccept += "Your fight has finished.";
        break;
    case PVP :
        stdAccept += "Successfully attacked another player.";
        break;
    case LOOT :
        stdAccept += "Successfully looted another player.";
        break;
    case START :
        stdAccept += "You've successfully started.";
        break;
    case ERROR :
        stdAccept += "You've successfully sent an error.";
        break;
    case ACCEPT :
        stdAccept += "You've successfully sent an accept.";
        break;
    case ROOM :
        stdAccept += "You've successfully sent an room.";
        break;
    case CHARACTER :
        stdAccept += "You've created a valid character.";
        our_widget->ui->characterButton->setVisible(false);
        our_widget->ui->characterButton->setEnabled(false);
        our_widget->ui->nameText->setReadOnly(true);
        our_widget->ui->descriptionText->setReadOnly(true);
        our_widget->ui->healthSpinBox->setReadOnly(true);
        our_widget->ui->regenSpinBox->setReadOnly(true);
        our_widget->ui->attackSpinBox->setReadOnly(true);
        our_widget->ui->defenseSpinBox->setReadOnly(true);
        our_widget->ui->roomSpinBox->setReadOnly(true);
        our_widget->ui->goldSpinBox->setReadOnly(true);
        our_widget->ui->joinBattleCheckBox->setEnabled(false);
        our_widget->ui->startButton->setVisible(true);
        our_widget->ui->startButton->setEnabled(true);
        our_widget->ui->nameText->setFocusPolicy(Qt::ClickFocus);
        our_widget->ui->descriptionText->setFocusPolicy(Qt::ClickFocus);
        our_widget->ui->healthSpinBox->setFocusPolicy(Qt::ClickFocus);
        our_widget->ui->regenSpinBox->setFocusPolicy(Qt::ClickFocus);
        our_widget->ui->attackSpinBox->setFocusPolicy(Qt::ClickFocus);
        our_widget->ui->defenseSpinBox->setFocusPolicy(Qt::ClickFocus);
        our_widget->ui->roomSpinBox->setFocusPolicy(Qt::ClickFocus);
        our_widget->ui->goldSpinBox->setFocusPolicy(Qt::ClickFocus);
        our_widget->ui->startButton->setFocus();
        break;
    case GAME :
        stdAccept += "You've successfully sent an game.";
        break;
    case LEAVE :
        stdAccept += "You've successfully sent an leave.";
        break;
    case CONNECTION :
        stdAccept += "You've successfully sent an connection.";
        break;
    case VERSION :
        stdAccept += "You've successfully sent an version.";
        break;
    default :
        stdAccept += "Invalid accept type received.";
        break;
    }

    QAccept = QString::fromStdString(stdAccept);
    our_widget->QAccept = QAccept;

    emitter_mutex.lock();
    emit our_widget->change_main_text(QAccept);
    emitter_mutex.unlock();

    delete our_accept;
    return;
}

void handle_version(int sfd, Widget* our_widget) {
    class lurk_version* our_version = new lurk_version();

    string STDVersion;
    QString QVersion;

    recv(sfd, our_version, 5, MSG_WAITALL);

    STDVersion = "[VERSION]: " + to_string(our_version->major) + "." + to_string(our_version->minor);
    QVersion = QString::fromStdString(STDVersion);
    our_widget->QVersion = QVersion;

    emitter_mutex.lock();
    emit our_widget->change_main_text(QVersion);
    emitter_mutex.unlock();

    delete our_version;
    return;
}

void handle_game(int sfd, Widget* our_widget) {
    class game* our_game = new game();

    string stdGame;
    QString QGame;

    recv(sfd, our_game, 7, MSG_WAITALL);
    our_game->description = (char*)calloc(our_game->description_length + 1, sizeof(char));
    recv(sfd, our_game->description, our_game->description_length, MSG_WAITALL);
    our_widget->initial_points = our_game->initial_points;
    our_widget->stat_limit = our_game->stat_limit;

    our_widget->ui->healthSpinBox->setMaximum(our_widget->stat_limit);
    our_widget->ui->regenSpinBox->setMaximum(our_widget->stat_limit);
    our_widget->ui->attackSpinBox->setMaximum(our_widget->stat_limit);
    our_widget->ui->defenseSpinBox->setMaximum(our_widget->stat_limit);
    our_widget->ui->roomSpinBox->setMaximum(our_widget->stat_limit);
    our_widget->ui->goldSpinBox->setMaximum(our_widget->stat_limit);

    stdGame = "[INITIAL POINTS]: " + to_string(our_game->initial_points) + "\n[STAT LIMIT]: " + to_string(our_game->stat_limit) + "\n" + our_game->description;
    QGame = QString::fromStdString(stdGame);
    our_widget->QGame = QGame;

    emitter_mutex.lock();
    emit our_widget->change_main_text(QGame);
    emitter_mutex.unlock();

    free(our_game->description);
    delete our_game;
    return;
}

void server_listener(int sfd, Widget* our_widget) {
    struct sockaddr_in server_interface;
    server_interface.sin_family = AF_INET;
    // listen port
    uint16_t lp;
    // protocol type
    uint8_t type;

    while(1) {
        // if we've attempted to connect to a server
        if(our_widget->ui->connectButton->isVisible() == false) {

            // ascii to network long; packed into sin_addr
            inet_aton((our_widget->listen_address).toStdString().c_str(), (struct in_addr*)&server_interface.sin_addr);
            // ascii to integer; stored in listen port
            lp = atoi((our_widget->listen_port).toStdString().c_str());
            // conversion of listen port from host byte order to network short
            server_interface.sin_port = htons(lp);

            while(1) {
                if(connect(sfd, (struct sockaddr*)&server_interface, sizeof(server_interface))) {
                    our_widget->ui->connectButton->setVisible(true);
                    our_widget->ui->addressText->setVisible(true);
                    our_widget->ui->portSpinBox->setVisible(true);
                    our_widget->ui->joinBattleCheckBox->setEnabled(false);
                    our_widget->ui->characterButton->setEnabled(false);

                    emitter_mutex.lock();
                    emit our_widget->change_main_text("Connection to target server failed, please try again.");
                    emitter_mutex.unlock();

                    break;
                } else {
                    while(1) {
                        // std::this_thread::sleep_for(std::chrono::milliseconds(250));
                        recv(sfd, &type, 1, MSG_PEEK);

                        switch(type) {
                        case VERSION:
                            protocol_mutex.lock();
                            handle_version(sfd, our_widget);
                            protocol_mutex.unlock();
                            break;
                        case GAME:
                            protocol_mutex.lock();
                            handle_game(sfd, our_widget);
                            protocol_mutex.unlock();
                            break;
                        case ACCEPT:
                            protocol_mutex.lock();
                            handle_accept(sfd, our_widget);
                            protocol_mutex.unlock();
                            break;
                        case CHARACTER:
                            protocol_mutex.lock();
                            handle_character(sfd, our_widget);
                            protocol_mutex.unlock();
                            break;
                        case ROOM:
                            protocol_mutex.lock();
                            handle_room(sfd, our_widget);
                            protocol_mutex.unlock();
                            break;
                        case CONNECTION:
                            protocol_mutex.lock();
                            handle_connection(sfd, our_widget);
                            protocol_mutex.unlock();
                            break;
                        case MESSAGE:
                            protocol_mutex.lock();
                            handle_incoming_message(sfd, our_widget);
                            protocol_mutex.unlock();
                            break;
                        case ERROR:
                            protocol_mutex.lock();
                            handle_error(sfd, our_widget);
                            protocol_mutex.unlock();
                            break;
                        }

                        continue;
                    }
                }
            }
        }
    }

    return;
}

int main(int argc, char* argv[]) {
    qRegisterMetaType<QTextBlock>();
    qRegisterMetaType<QTextCursor>();

    QApplication a(argc, argv);
    Widget main_window;

    QObject::connect((const QObject*)&main_window,
                     SIGNAL(change_main_text(QString)),
                     main_window.ui->mainText,
                     SLOT(appendPlainText(QString))
                     );

    QObject::connect((const QObject*)&main_window,
                     SIGNAL(change_neighbor_board(QString)),
                     main_window.ui->neighborBoardText,
                     SLOT(appendPlainText(QString))
                     );

    QObject::connect((const QObject*)&main_window,
                     SIGNAL(change_message_board(QString)),
                     main_window.ui->messageBoardText,
                     SLOT(appendPlainText(QString))
                     );

    QObject::connect((const QObject*)&main_window,
                     SIGNAL(reset_neighbor_board()),
                     main_window.ui->neighborBoardText,
                     SLOT(clear())
                     );

    QObject::connect((const QObject*)&main_window,
                     SIGNAL(aboutToQuit()),
                     (const QObject*)&main_window,
                     SLOT(on_leaveButton_clicked())
                     );

    int server_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    main_window.sfd = server_file_descriptor;
    
    QThread *server_thread = QThread::create(server_listener, server_file_descriptor, &main_window);
    server_thread->setObjectName("Lurk Server listener");

    main_window.show();
    server_thread->start();

    return a.exec();
}
