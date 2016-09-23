/*
    This file is part of the telepathy-morse connection manager.
    Copyright (C) 2016 Alexandr Akulich <akulichalexander@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "identifier.hpp"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
static const QString c_userPrefix = QLatin1String("user");
static const QString c_chatPrefix = QLatin1String("chat");
#else
static const QLatin1String c_userPrefix = QLatin1String("user");
static const QLatin1String c_chatPrefix = QLatin1String("chat");
#endif

MorseIdentifier::MorseIdentifier() :
    m_userId(0),
    m_chatId(0)
{

}

bool MorseIdentifier::operator==(const MorseIdentifier &identifier) const
{
    return (m_userId == identifier.m_userId)
            && (m_chatId == identifier.m_chatId)
            && (m_phoneNumber == identifier.m_phoneNumber);
}

bool MorseIdentifier::operator!=(const MorseIdentifier &identifier) const
{
    return !(*this == identifier);
}

bool MorseIdentifier::isNull() const
{
    return phoneNumber().isEmpty() && !isTgIdentifier();
}

TelegramNamespace::Peer MorseIdentifier::toPeer() const
{
    if (m_chatId) {
        return TelegramNamespace::Peer(m_chatId, TelegramNamespace::Peer::Chat);
    } else if (m_userId) {
        return TelegramNamespace::Peer(m_userId);
    }

    return TelegramNamespace::Peer();
}

QString MorseIdentifier::phoneNumber() const
{
    return m_phoneNumber;
}

MorseIdentifier MorseIdentifier::fromPeer(const TelegramNamespace::Peer &peer)
{
    MorseIdentifier id;

    if (peer.type == TelegramNamespace::Peer::User) {
        id.m_userId = peer.id;
    } else {
        id.m_chatId = peer.id;
    }

    return id;
}

MorseIdentifier MorseIdentifier::fromChatId(quint32 chatId)
{
    MorseIdentifier id;
    id.m_chatId = chatId;
    return id;
}

MorseIdentifier MorseIdentifier::fromUserId(quint32 userId)
{
    MorseIdentifier id;
    id.m_userId = userId;
    return id;
}

MorseIdentifier MorseIdentifier::fromUserInChatId(quint32 chatId, quint32 userId)
{
    MorseIdentifier id;
    id.m_userId = userId;
    id.m_chatId = chatId;
    return id;
}

QString MorseIdentifier::toString() const
{
    QString result;

    if (!m_phoneNumber.isEmpty()) {
        return QLatin1String("tel:+") + m_phoneNumber;
    }

    if (m_chatId) {
        result = c_chatPrefix + QString::number(m_chatId);
    }

    if (m_userId) {
        result = c_userPrefix + QString::number(m_userId);
    }

    Q_ASSERT(!result.isEmpty());

    return result;
}

MorseIdentifier MorseIdentifier::fromString(const QString &string)
{
    MorseIdentifier id;

    if (string.startsWith(QLatin1String("tel:+"))) {
        id.m_phoneNumber = string.mid(5);
        // Phone number without leading + and ().
        return id;
    }

    // Possible schemes: user1234, chat1234, user1234_chat1234

    int chatOffset = string.indexOf(c_chatPrefix);

    if (string.startsWith(c_userPrefix)) {
        int length = chatOffset < 0 ? -1 : chatOffset - c_userPrefix.size() - 1;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        id.m_userId = string.mid(c_userPrefix.size(), length).toUInt();
    }

    if (chatOffset >= 0) {
        id.m_chatId = string.mid(c_chatPrefix.size() + chatOffset).toUInt();
#else
        id.m_userId = string.midRef(c_userPrefix.size(), length).toUInt();
    }

    if (chatOffset >= 0) {
        id.m_chatId = string.midRef(c_chatPrefix.size() + chatOffset).toUInt();
#endif
    }

    return id;
}
