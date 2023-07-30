#include "widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    setTabOrder(ui->addressText, ui->portSpinBox);
    setTabOrder(ui->portSpinBox, ui->connectButton);
    setTabOrder(ui->connectButton, ui->nameText);
    setTabOrder(ui->nameText, ui->descriptionText);
    setTabOrder(ui->descriptionText, ui->healthSpinBox);
    setTabOrder(ui->healthSpinBox, ui->regenSpinBox);
    setTabOrder(ui->regenSpinBox, ui->attackSpinBox);
    setTabOrder(ui->attackSpinBox, ui->defenseSpinBox);
    setTabOrder(ui->defenseSpinBox, ui->roomSpinBox);
    setTabOrder(ui->roomSpinBox, ui->goldSpinBox);
    setTabOrder(ui->goldSpinBox, ui->characterButton);
    setTabOrder(ui->characterButton, ui->startButton);
    setTabOrder(ui->startButton, ui->changeroomSpinBox);
    setTabOrder(ui->changeroomSpinBox, ui->changeroomButton);
    setTabOrder(ui->changeroomButton, ui->fightButton);
    setTabOrder(ui->fightButton, ui->pvpButton);
    setTabOrder(ui->pvpButton, ui->lootButton);
    setTabOrder(ui->lootButton, ui->targetText);
    setTabOrder(ui->targetText, ui->cancelTargetButton);
    setTabOrder(ui->cancelTargetButton, ui->leaveButton);
    setTabOrder(ui->leaveButton, ui->messagePromptButton);
    setTabOrder(ui->messagePromptButton, ui->messageRecipientText);
    setTabOrder(ui->messageRecipientText, ui->messageText);
    setTabOrder(ui->messageText, ui->cancelMessageButton);

    ui->addressText->setFocus();

    ui->messagePromptButton->setEnabled(false);

    ui->startButton->setVisible(false);
    ui->startButton->setEnabled(false);

    ui->changeroomButton->setVisible(false);
    ui->changeroomButton->setEnabled(false);

    ui->fightButton->setVisible(false);
    ui->fightButton->setEnabled(false);

    ui->pvpButton->setVisible(false);
    ui->pvpButton->setEnabled(false);

    ui->lootButton->setVisible(false);
    ui->lootButton->setEnabled(false);

    ui->messageRecipientText->setVisible(false);

    ui->messageText->setVisible(false);

    ui->targetText->setVisible(false);
    ui->targetText->setEnabled(false);

    ui->changeroomSpinBox->setVisible(false);
    ui->changeroomSpinBox->setEnabled(false);

    ui->characterButton->setEnabled(false);

    ui->cancelTargetButton->setVisible(false);
    ui->cancelMessageButton->setVisible(false);

    name = QString();
    description = QString();
    message_recipient = QString();
    message = QString();
    auto_pve = false;
    attack = defense = regen = gold = room = 0;
    health = 0;

    listen_address = "74.118.22.194";
    listen_port = "";
}

Widget::~Widget() { delete ui; }

void Widget::on_characterButton_clicked() {
    if((name != "") && (description != "") && ((attack + defense + regen) <= initial_points) && ((attack + defense + regen) > 0)) {
        class lurk_character* our_character = new lurk_character();

        uint name_length = name.toStdString().length();
        if(name_length > 32) { name_length = 32; }
        string stdName = name.toStdString();
        strncpy(our_character->player_name, stdName.c_str(), name_length);
        our_character->player_name[name_length] = '\0';;

        our_character->reserved = 0;
        our_character->ready = 1;
        our_character->started = 0;
        our_character->monster = 0;
        if(ui->joinBattleCheckBox->isChecked()) { our_character->join_battle = 1; }
        else { our_character->join_battle = 0; }

        our_character->alive = 1;
        our_character->attack_damage = attack;
        our_character->defense_rating = defense;
        our_character->regeneration = regen;
        our_character->health_points = health;
        our_character->gold_count = gold;
        our_character->current_room = room;

        uint description_length = description.toStdString().length();
        if(description_length > 65535) { description_length = 65535; }
        our_character->description_length = description_length;

        string stdDescription = description.toStdString();
        our_character->description = (char*)calloc(description_length + 1, sizeof(char));
        strncpy(our_character->description, stdDescription.c_str(), description_length);

        write(sfd, our_character, 48);
        write(sfd, our_character->description, description_length);

        free(our_character->description);
        delete(our_character);
    }

    return;
}

void Widget::on_nameText_textChanged() {
    name = ui->nameText->toPlainText();

    return;
}

void Widget::on_joinBattleCheckBox_stateChanged(int arg1) {
    auto_pve = (arg1 == 2)? true : false;

    return;
}

void Widget::on_attackSpinBox_valueChanged(int arg1) {
    attack = arg1;

    return;
}

void Widget::on_defenseSpinBox_valueChanged(int arg1) {
    defense = arg1;

    return;
}

void Widget::on_regenSpinBox_valueChanged(int arg1) {
    regen = arg1;

    return;
}

void Widget::on_descriptionText_textChanged() {
    description = ui->descriptionText->toPlainText();

    return;
}

void Widget::on_messagePromptButton_clicked() {
    if(message_recipient == "" && message == "") { ui->messagePromptButton->setEnabled(false); }
    else if(message_recipient != "" && message != "") {
        class lurk_message* our_message = new lurk_message();
        our_message->message_length = message.toStdString().length();

        strncpy(our_message->recipient_name, message_recipient.toStdString().c_str(), message_recipient.toStdString().length());
        our_message->recipient_name[message_recipient.toStdString().length()] = '\0';

        strncpy(our_message->sender_name, name.toStdString().c_str(), name.toStdString().length());
        our_message->sender_name[name.toStdString().length()] = '\0';

        our_message->narration_marker[0] = 0;
        our_message->narration_marker[1] = 0;
        our_message->message = (char*)calloc(our_message->message_length + 1, sizeof(char));
        strncpy(our_message->message, message.toStdString().c_str(), our_message->message_length);

        write(sfd, our_message, 67);
        write(sfd, our_message->message, our_message->message_length);

        ui->messageRecipientText->setVisible(false);
        ui->messageRecipientText->setEnabled(false);
        ui->messageRecipientText->setPlainText("");

        ui->messageText->setVisible(false);
        ui->messageText->setEnabled(false);
        ui->messageText->setPlainText("");

        message_recipient = "";
        message = "";

        ui->cancelMessageButton->setVisible(false);

        ui->messagePromptButton->setEnabled(true);

        free(our_message->message);
        delete our_message;
        return;
    }

    ui->messageRecipientText->setVisible(true);
    ui->messageRecipientText->setEnabled(true);
    ui->messageRecipientText->setFocus();

    ui->messageText->setVisible(true);
    ui->messageText->setEnabled(true);

    ui->cancelMessageButton->setVisible(true);

    return;
}

void Widget::on_leaveButton_clicked() {
    class lurk_leave* leave = new lurk_leave();
    write(sfd, leave, 1);

    delete leave;
    QWidget::close();
    return;
}

void Widget::on_messageRecipientText_textChanged() {
    message_recipient = ui->messageRecipientText->toPlainText();

    if(message_recipient == "") { ui->messagePromptButton->setEnabled(false); }
    if(message_recipient != "" && message != "") { ui->messagePromptButton->setEnabled(true); }

    return;
}

void Widget::on_messageText_textChanged() {
    message = ui->messageText->toPlainText();

    if(message == "") { ui->messagePromptButton->setEnabled(false); }
    if(message_recipient != "" && message != "") { ui->messagePromptButton->setEnabled(true); }

    return;
}

void Widget::on_fightButton_clicked() {
    class fight* our_fight = new fight();
    write(sfd, our_fight, 1);

    delete our_fight;
    return;
}

void Widget::on_pvpButton_clicked() {
    if(target != "") {
        class pvp_fight* our_pvp = new pvp_fight();
        strncpy(our_pvp->player_name, target.toStdString().c_str(), target.toStdString().length());
        our_pvp->player_name[target.toStdString().length()] = '\0';
        write(sfd, our_pvp, 33);

        ui->targetText->setPlainText("");
        target = "";

        ui->changeroomButton->setVisible(true);
        ui->changeroomSpinBox->setVisible(true);

        if(ui->fightButton->isVisible() == false) { ui->fightButton->setVisible(true); }
        if(ui->pvpButton->isVisible() == false) { ui->pvpButton->setVisible(true); }
        if(ui->lootButton->isVisible() == false) { ui->lootButton->setVisible(true); }

        ui->targetText->setVisible(false);
        ui->targetText->setEnabled(false);

        ui->cancelTargetButton->setVisible(false);

        delete our_pvp;
        return;
    }

    ui->changeroomButton->setVisible(false);
    ui->changeroomSpinBox->setVisible(false);

    ui->fightButton->setVisible(false);

    ui->lootButton->setVisible(false);

    ui->cancelTargetButton->setVisible(true);

    ui->targetText->setVisible(true);
    ui->targetText->setEnabled(true);
    ui->targetText->setFocus();

    return;
}

void Widget::on_lootButton_clicked() {
    if(target != "") {
        class loot* our_loot = new loot();
        strncpy(our_loot->player_name, target.toStdString().c_str(), target.toStdString().length());
        our_loot->player_name[target.toStdString().length()] = '\0';
        write(sfd, our_loot, 33);

        ui->targetText->setPlainText("");
        target = "";

        ui->changeroomButton->setVisible(true);
        ui->changeroomSpinBox->setVisible(true);

        if(ui->fightButton->isVisible() == false) { ui->fightButton->setVisible(true); }
        if(ui->pvpButton->isVisible() == false) { ui->pvpButton->setVisible(true); }
        if(ui->lootButton->isVisible() == false) { ui->lootButton->setVisible(true); }

        ui->targetText->setVisible(false);
        ui->targetText->setEnabled(false);

        ui->cancelTargetButton->setVisible(false);

        delete our_loot;
        return;
    }

    ui->changeroomButton->setVisible(false);
    ui->changeroomSpinBox->setVisible(false);
    ui->fightButton->setVisible(false);
    ui->pvpButton->setVisible(false);
    ui->cancelTargetButton->setVisible(true);
    ui->targetText->setVisible(true);
    ui->targetText->setEnabled(true);
    ui->targetText->setFocus();

    return;
}

void Widget::on_startButton_clicked() {
    class lurk_start* our_start = new lurk_start();
    write(sfd, our_start, 1);

    ui->messagePromptButton->setEnabled(true);
    ui->changeroomButton->setVisible(true);
    ui->changeroomButton->setEnabled(true);
    ui->fightButton->setVisible(true);
    ui->fightButton->setEnabled(true);
    ui->pvpButton->setVisible(true);
    ui->pvpButton->setEnabled(true);
    ui->lootButton->setVisible(true);
    ui->lootButton->setEnabled(true);
    ui->startButton->setVisible(false);
    ui->startButton->setEnabled(false);
    ui->changeroomSpinBox->setVisible(true);
    ui->changeroomSpinBox->setEnabled(true);
    ui->changeroomSpinBox->setFocus();

    delete our_start;
    return;
}

void Widget::on_changeroomButton_clicked() {
    class changeroom* new_room = new changeroom();
    new_room->room_number = destination_room;
    write(sfd, new_room, 3);

    neighbor_map.clear();
    ui->neighborBoardText->clear();

    delete new_room;
    return;
}

void Widget::on_healthSpinBox_valueChanged(int arg1) {
    health = arg1;
    return;
}

void Widget::on_roomSpinBox_valueChanged(int arg1) {
    room = arg1;
    return;
}

void Widget::on_goldSpinBox_valueChanged(int arg1) {
    gold = arg1;
    return;
}

void Widget::on_addressText_textChanged() {
    listen_address = ui->addressText->toPlainText();
    if(listen_address != "" && listen_port != "") { ui->connectButton->setEnabled(true); }
    else { ui->connectButton->setEnabled(false); }

    return;
}

void Widget::on_portSpinBox_valueChanged(int arg1) {
    listen_port = QString::fromStdString(to_string(arg1));
    if(listen_port != "" && listen_address != "") { ui->connectButton->setEnabled(true); }
    else { ui->connectButton->setEnabled(false); }

    return;
}

void Widget::on_connectButton_clicked() {
    if(ui->addressText->toPlainText() == "" || ui->portSpinBox->value() == 0) { return; }

    ui->connectButton->setVisible(false);
    ui->addressText->setVisible(false);
    ui->portSpinBox->setVisible(false);
    ui->joinBattleCheckBox->setEnabled(true);
    ui->characterButton->setEnabled(true);

    ui->nameText->setFocus();

    return;
}

void Widget::on_changeroomSpinBox_valueChanged(int arg1) {
    destination_room = arg1;
    return;
}

void Widget::on_targetText_textChanged() {
    target = ui->targetText->toPlainText();
    return;
}

void Widget::on_cancelTargetButton_clicked() {
    ui->targetText->setPlainText("");
    target = "";

    ui->changeroomButton->setVisible(true);
    ui->changeroomSpinBox->setVisible(true);

    if(ui->fightButton->isVisible() == false) { ui->fightButton->setVisible(true); }
    if(ui->pvpButton->isVisible() == false) { ui->pvpButton->setVisible(true); }
    if(ui->lootButton->isVisible() == false) { ui->lootButton->setVisible(true); }

    ui->targetText->setVisible(false);
    ui->targetText->setEnabled(false);

    ui->cancelTargetButton->setVisible(false);

    ui->changeroomSpinBox->setFocus();

    return;
}

void Widget::on_cancelMessageButton_clicked() {
    message_recipient = message = "";

    ui->messageRecipientText->setVisible(false);
    ui->messageRecipientText->setEnabled(true);
    ui->messageRecipientText->setPlainText("");

    ui->messageText->setVisible(false);
    ui->messageText->setEnabled(true);
    ui->messageText->setPlainText("");

    ui->cancelMessageButton->setVisible(false);

    ui->messagePromptButton->setEnabled(true);

    ui->changeroomSpinBox->setFocus();

    return;
}

void Widget::keyPressEvent(QKeyEvent *event) {
    QString focused_object = focusWidget()->objectName();

    if(event->key() == Qt::Key_Return) {
        if(focused_object == "connectButton") { on_connectButton_clicked(); return; }
        if(focused_object == "characterButton") { on_characterButton_clicked(); return; }
        if(focused_object == "startButton") { on_startButton_clicked(); return; }
        if(focused_object == "changeroomButton") { on_changeroomButton_clicked(); return; }
        if(focused_object == "fightButton") { on_fightButton_clicked(); return; }
        if(focused_object == "pvpButton") { on_pvpButton_clicked(); return; }
        if(focused_object == "lootButton") { on_lootButton_clicked(); return; }
        if(focused_object == "cancelTargetButton") { on_cancelTargetButton_clicked(); return; }
        if(focused_object == "leaveButton") { on_leaveButton_clicked(); return; }
        if(focused_object == "messagePromptButton") { on_messagePromptButton_clicked(); return; }
        if(focused_object == "cancelMessageButton") { on_cancelMessageButton_clicked(); return; }
    }

    if(event->key() == Qt::Key_Escape) {
        if(focused_object == "targetText") { on_cancelTargetButton_clicked(); return; }
        if(focused_object == "messageRecipientText") { on_cancelMessageButton_clicked(); return; }
        if(focused_object == "messageText") { on_cancelMessageButton_clicked(); return; }
    }

    return;
}
