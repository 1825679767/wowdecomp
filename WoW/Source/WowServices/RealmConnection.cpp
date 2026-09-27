#include "RealmConnection.h"

// STATIC: 0x00AB953C
TSList<RealmConnection::REALMCONNECTIONNODE> RealmConnection::s_allRealmConnections;

// STATIC: 0x00B38CA8
SCritSect RealmConnection::s_allRealmConnectionsCrit;

// VERIFY: 1d949124 auto
// INLINED
RealmConnection::REALMCONNECTIONNODE::REALMCONNECTIONNODE() {
    this->m_connection = nullptr;
}

// VERIFY: 0d14ec4d manual
// OFFSET: 0x004650E0
RealmConnection::RealmConnection(RealmResponse* realmResponse) : m_realmResponse(realmResponse), m_authenticated(0), m_queuePosition(0), m_freeCharacterMigration(0), m_billingTimeRemaining(0), m_billingTimeRested(0), m_billingFlags(0), m_accountExpansion(0), unk_2F30(0), unk_2F34(0), unk_2F38(0) {
    this->Initialize();

    this->SetMessageHandler(SMSG_AUTH_CHALLENGE, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_AUTH_RESPONSE, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_ADDON_INFO, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_ENUM_CHARACTERS_RESULT, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_CHAR_CREATE, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_CHARACTER_LOGIN_FAILED, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_LOGOUT_COMPLETE, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_LOGOUT_CANCEL_ACK, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_LOGOUT_RESPONSE, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_DELETE_CHAR, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_CACHE_VERSION, RealmConnection::MessageHandler, this);

    RealmConnection::s_allRealmConnectionsCrit.Enter();
    RealmConnection::REALMCONNECTIONNODE* node = RealmConnection::s_allRealmConnections.NewNode(2, 0, 0);
    node->m_connection = this;
    RealmConnection::s_allRealmConnectionsCrit.Leave();
}

// VERIFY: 1d949124 auto
// OFFSET: 0x00464BC0
void RealmConnection::PollNet(const void* data, void* param) {
    RealmConnection::s_allRealmConnectionsCrit.Enter();

    RealmConnection::REALMCONNECTIONNODE* node = RealmConnection::s_allRealmConnections.Head();

    while (!TSList<RealmConnection::REALMCONNECTIONNODE, TSGetLink<RealmConnection::REALMCONNECTIONNODE> >::IsEnd(node)) {
        RealmConnection::REALMCONNECTIONNODE* next = RealmConnection::s_allRealmConnections.RawNext(node);
        node->m_connection->PollEventQueue();
        node = next;
    }

    RealmConnection::s_allRealmConnectionsCrit.Leave();
}
