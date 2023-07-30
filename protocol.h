#include "widget.h"

using namespace std;

class fight {
public:
    uint8_t type = 3;

    fight() { }
    bool receive(int cfd) {
        if(1 != read(cfd, this, 1)) {
            printf("Failed to receive() FIGHT from descriptor %d.\n", cfd);
            return false;
        }
        return true;
    }
}__attribute__((packed));

class pvp_fight {
public:
    uint8_t type = 4;
    char player_name[32];

    pvp_fight() { }
    bool receive(int cfd) {
        if(33 != recv(cfd, this, 33, MSG_WAITALL)) {
            printf("Failed to receive() PVP from descriptor %d.\n", cfd);
            return false;
        }
        return true;
    }
}__attribute__((packed));

class loot {
public:
    uint8_t type = 5;
    char player_name[32];

    loot() { }
    bool receive(int cfd) {
        if(33 != recv(cfd, this, 33, MSG_WAITALL)) {
            printf("Failed to receive() LOOT from descriptor %d.\n", cfd);
            return false;
        }
        return true;
    }
}__attribute__((packed));

class lurk_error {
public:
    uint8_t type = 7;
    uint8_t error_code;
    uint16_t error_length;
    char *error_message = 0;

    lurk_error() { }
    lurk_error(uint8_t ec, uint8_t el, char *em) : error_code(ec), error_length(el) {
        error_message = (char *)malloc(error_length);
        strncpy(error_message, em, error_length);
    }
    bool send(int cfd) {
        if(4 != write(cfd, this, 4)) { return false; }
        if(error_length != write(cfd, error_message, error_length)) { return false; }
        return true;
    }
    // ~error() { if(error_message) { free(error_message); } }
}__attribute__((packed));

class lurk_accept {
public:
    uint8_t type = 8;
    uint8_t accepted_type;

    lurk_accept() { }
    lurk_accept(uint8_t at) : accepted_type(at) { }
    bool send(int cfd) {
        if(2 != write(cfd, this, 2)) { return false; }
        return true;
    }
    bool receive(int sfd) {
        if(2 != read(sfd, this, 2)) { return false; }
        return true;
    }
}__attribute__((packed));

class lurk_room {
public:
    uint8_t type = 9;
    uint16_t room_number;
    char room_name[32];
    uint16_t description_length;
    char *description = 0;

    lurk_room() { }
    lurk_room(uint16_t rnum, char *rname, uint16_t dl, char *rdesc) : room_number(rnum), description_length(dl) {
        strncpy(room_name, rname, sizeof(room_name));
        description = (char *)malloc(description_length);
        strncpy(description, rdesc, description_length);
    }
    bool send(int cfd) {
        if(37 != write(cfd, this, 37)) { return false; }
        if(description_length != write(cfd, description, description_length)) { return false; }
        return true;
    }
    bool send_connection(int cfd) {
        type = 13;
        if(37 != write(cfd, this, 37)) { return false; }
        if(description_length != write(cfd, description, description_length)) { return false; }
        type = 9;
        return true;
    }
    bool receive(int sfd) {
        if(37 != recv(sfd, this, 37, MSG_WAITALL)) { return false; }
        description = (char*)malloc(description_length);
        if(description_length != recv(sfd, this, description_length, MSG_WAITALL)) { return false; }
        return true;
    }
    //_room() { if(description) { free(description); } }
}__attribute__((packed));

class lurk_character {
public:
    uint8_t type = 10;
    char player_name[32];
    uint8_t reserved:3;
    uint8_t ready:1;
    uint8_t started:1;
    uint8_t monster:1;
    uint8_t join_battle:1;
    uint8_t alive:1;
    uint16_t attack_damage;
    uint16_t defense_rating;
    uint16_t regeneration;
    uint16_t health_points;
    uint16_t gold_count;
    uint16_t current_room;
    uint16_t description_length;
    char *description = 0;

    lurk_character() { }
    lurk_character(char *pn, uint8_t jb, uint16_t ad, uint16_t dr, uint16_t rg, uint16_t hp,
                   uint16_t gc, uint16_t cr, uint16_t dl, char *pd) :
        reserved(0), ready(1), started(1), monster(1), join_battle(jb), alive(1),
        attack_damage(ad), defense_rating(dr), regeneration(rg), health_points(hp),
        gold_count(gc), current_room(cr), description_length(dl) {
        strncpy(player_name, pn, sizeof(player_name));
        description = (char *)malloc(description_length);
        strncpy(description, pd, description_length);
    }
    bool receive(int cfd) {
        if(48 != recv(cfd, this, 48, MSG_WAITALL)) { return false; }
        description = (char *)malloc(description_length);
        if(description_length != recv(cfd, description, description_length, MSG_WAITALL)) {
            printf("Failed to receive() CHARACTER description from descriptor %d\n", cfd);
            return false;
        }
        return true;
    }
    bool send(int cfd) {
        if(48 != write(cfd, this, 48)) { return false; }
        if(description_length != write(cfd, description, description_length)) { return false; }
        return true;
    }
    // //_character() { if(description) { free(description); } }
}__attribute__((packed));

class lurk_message {
public:
    uint8_t type = 1;
    uint16_t message_length;
    char recipient_name[32];
    char sender_name[30];
    char narration_marker[2];
    char *message = 0;

    lurk_message() { }
    lurk_message(uint16_t ml, char *rn, char *sn, bool nm, char *m) : message_length(ml) {
        strncpy(recipient_name, rn, sizeof(recipient_name));
        strncpy(sender_name, sn, sizeof(sender_name));
        if(nm) {
            narration_marker[0] = 0;
            narration_marker[1] = 1;
        }
        message = (char *)malloc(message_length);
        strncpy(message, m, message_length);
    }
    bool receive(int cfd) {
        if(67 != recv(cfd, this, 67, MSG_WAITALL)) {
            return false;
        }
        message = (char *)malloc(message_length);
        if(message_length != recv(cfd, message, message_length, MSG_WAITALL)) {
            return false;
        }
        return true;
    }
    bool send(int cfd) {
        if(67 != write(cfd, this, 67))
            return false;
        if(message_length != write(cfd, message, message_length))
            return false;
        return true;
    }
    //_message() { if(message) { free(message); } }
}__attribute__((packed));

class changeroom {
public:
    uint8_t type = 2;
    uint16_t room_number;

    changeroom() { }
    bool receive(int cfd) {
        if(3 != read(cfd, this, 3)) { return false; }
        return true;
    }
    bool send(int cfd, class lurk_room *destination, class lurk_character *client_character, map<int, lurk_character*> character_map, vector<lurk_character*> npcs, vector<lurk_room*> rooms) {
        // send the desired lurk_room to ourselves
        if(!destination->send(cfd)) {
            printf("CHANGEROOM destination send() to descriptor %d failed.\n", cfd);
            return false;
        }
        for(auto& i : character_map) {
            if(i.second) {
                if(i.second->current_room == client_character->current_room && strcmp(i.second->player_name, client_character->player_name)) {
                    // write neighboring players to ourselves
                    if(!i.second->send(cfd)) {
                        printf("CHANGEROOM neighbor send() to descriptor %d failed.\n", cfd);
                        return false;
                    }
                    // if the other player in the lurk_room is still connected
                    if(i.first > 0) {
                        // then write ourselves to them
                        if(!client_character->send(i.first)) {
                            printf("CHANGEROOM client_character send() to descriptor %d failed.\n", i.first);
                            return false;
                        }
                    }
                }
            }
        }
        for(auto& i : npcs) {
            if(i->current_room == client_character->current_room) {
                // write neighboring NPCs to ourselves
                if(!i->send(cfd)) {
                    printf("CHANGEROOM NPC send() to descriptor %d failed.\n", cfd);
                    return false;
                }
            }
        }
        // send the connecting rooms to ourselves
        for(auto& i : rooms) {
            if(i->room_number == (client_character->current_room + 1) || i->room_number == (client_character->current_room - 1)) {
                if(!i->send_connection(cfd)) {
                    printf("CHANGEROOM send_connection() to descriptor %d failed.\n", cfd);
                    return false;
                }
            }
        }
        // send our updated lurk_character to ourselves
        if(!client_character->send(cfd)) {
            printf("CHANGEROOM client_character send() to descriptor %d failed.\n", cfd);
            return false;
        }
        return true;
    }
}__attribute__((packed));

class lurk_start {
public:
    uint8_t type = 6;

    lurk_start() { }
    bool send(int sfd) {
        if(1 != write(sfd, this, 1)) { return false; }
        return true;
    }
}__attribute__((packed));

class game {
public:
    uint8_t type = 11;
    uint16_t initial_points;
    uint16_t stat_limit;
    uint16_t description_length;
    char *description = 0;

    game() { }
    game(uint16_t ip, uint16_t sl, uint16_t dl, char *d) : initial_points(ip), stat_limit(sl), description_length(dl) {
        description = (char *)malloc(description_length);
        strncpy(description, d, description_length);
    }
    bool send(int cfd) {
        if(7 != write(cfd, this, 7))
            return false;
        if(description_length != write(cfd, description, description_length))
            return false;
        return true;
    }
    bool receive(int sfd) {
        recv(sfd, this, 7, MSG_WAITALL);
        description = (char *)malloc(description_length);
        recv(sfd, this, description_length, MSG_WAITALL);
        return true;
    }
    // ~game() { if(description) free(description); }
}__attribute__((packed));

class lurk_version {
public:
    uint8_t type = 14;
    uint8_t major;
    uint8_t minor;
    uint16_t extension_length = 0;

    // for some reason, "lurk_version();" does not work with QT, compiler notes it as an "undefined reference"
    lurk_version() { }
    lurk_version(uint8_t maj, uint8_t min) : major(maj), minor(min) { }
    bool send(int cfd) {
        return 5 == write(cfd, this, 5);
    }
    bool receive(int sfd) {
        return 5 == recv(sfd, this, 5, MSG_WAITALL);
    }
}__attribute__((packed));

class lurk_leave {
public:
    uint8_t type = 12;

    lurk_leave() { }
}__attribute__((packed));
