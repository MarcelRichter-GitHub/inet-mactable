//
// Copyright (C) 2005 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "inet/common/ProtocolUtils.h"

namespace inet {

void pushEncapsulationProtocolReq(Packet *packet, const Protocol *protocol)
{
    auto encapsulationProtocolReq = packet->addTagIfAbsent<EncapsulationProtocolReq>();
    encapsulationProtocolReq->appendProtocol(protocol);
}

const Protocol *popEncapsulationProtocolReq(Packet *packet)
{
    if (auto encapsulationProtocolReq = packet->findTagForUpdate<EncapsulationProtocolReq>()) {
        auto encapsulationProtocol = encapsulationProtocolReq->getProtocol(0);
        encapsulationProtocolReq->eraseProtocol(0);
        if (encapsulationProtocolReq->getProtocolArraySize() == 0)
            packet->removeTag<EncapsulationProtocolReq>();
        return encapsulationProtocol;
    }
    else
        return nullptr;
}

INET_API bool hasEncapsulationProtocolReq(Packet *packet, const Protocol *protocol)
{
    if (auto encapsulationProtocolReq = packet->addTagIfAbsent<EncapsulationProtocolReq>()) {
        for (int i = 0; i < encapsulationProtocolReq->getProtocolArraySize(); i++)
            if (encapsulationProtocolReq->getProtocol(i) == protocol)
                return true;
    }
    return false;
}

void ensureEncapsulationProtocolReq(Packet *packet, const Protocol *protocol)
{
    if (!hasEncapsulationProtocolReq(packet, protocol))
        pushEncapsulationProtocolReq(packet, protocol);
}

void pushEncapsulationProtocolInd(Packet *packet, const Protocol *protocol)
{
    auto encapsulationProtocolInd = packet->addTagIfAbsent<EncapsulationProtocolInd>();
    encapsulationProtocolInd->appendProtocol(protocol);
}

const Protocol *popEncapsulationProtocolInd(Packet *packet)
{
    if (auto encapsulationProtocolInd = packet->findTagForUpdate<EncapsulationProtocolInd>()) {
        auto encapsulationProtocol = encapsulationProtocolInd->getProtocol(0);
        encapsulationProtocolInd->eraseProtocol(0);
        if (encapsulationProtocolInd->getProtocolArraySize() == 0)
            packet->removeTag<EncapsulationProtocolInd>();
        return encapsulationProtocol;
    }
    else
        return nullptr;
}

} // namespace inet
