//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "inet/common/packet/serializer/ChunkSerializerRegistry.h"
#include "inet/common/serializer/ethernet/EthernetHeaderSerializer.h"
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#include "inet/linklayer/ethernet/EtherPhyFrame.h"

namespace inet {

namespace serializer {

Register_Serializer(EtherFrame, EthernetMacHeaderSerializer);
Register_Serializer(EthernetIIFrame, EthernetMacHeaderSerializer);
Register_Serializer(EtherFrameWithLLC, EthernetMacHeaderSerializer);
Register_Serializer(EtherFrameWithSNAP, EthernetMacHeaderSerializer);
Register_Serializer(EthernetPadding, EthernetPaddingSerializer);
Register_Serializer(EthernetFcs, EthernetFcsSerializer);
Register_Serializer(EtherPhyFrame, EthernetPhyHeaderSerializer);

void EthernetMacHeaderSerializer::serialize(MemoryOutputStream& stream, const std::shared_ptr<Chunk>& chunk) const
{
    const auto& ethernetHeader = std::static_pointer_cast<const EtherFrame>(chunk);
    stream.writeMACAddress(ethernetHeader->getDest());
    stream.writeMACAddress(ethernetHeader->getSrc());
    if (auto ethernetIIHeader = std::dynamic_pointer_cast<const EthernetIIFrame>(ethernetHeader)) {
        uint16_t ethType = ethernetIIHeader->getEtherType();
        stream.writeUint16(ethType);
    }
    else if (auto ethernetHeaderWithLLC = std::dynamic_pointer_cast<const EtherFrameWithLLC>(ethernetHeader)) {
        stream.writeUint16(ethernetHeaderWithLLC->getPayloadLength());
        stream.writeByte(ethernetHeaderWithLLC->getSsap());
        stream.writeByte(ethernetHeaderWithLLC->getDsap());
        stream.writeByte(ethernetHeaderWithLLC->getControl());
        if (auto frame = std::dynamic_pointer_cast<const EtherFrameWithSNAP>(ethernetHeader)) {
            stream.writeByte(frame->getOrgCode() >> 16);
            stream.writeByte(frame->getOrgCode() >> 8);
            stream.writeByte(frame->getOrgCode());
            stream.writeUint16(frame->getLocalcode());
        }
        else {
            auto x = ethernetHeaderWithLLC.get();
            if (typeid(*x) != typeid(EtherFrameWithLLC))
                throw cRuntimeError("Cannot serializer '%s'", ethernetHeader->getClassName());
        }
    }
    else if (auto pauseFrame = std::dynamic_pointer_cast<const EtherPauseFrame>(ethernetHeader)) {
        stream.writeUint16(0x8808);
        stream.writeUint16(0x0001);
        stream.writeUint16(pauseFrame->getPauseTime());
    }
    else
        throw cRuntimeError("Cannot serialize '%s'", ethernetHeader->getClassName());
}

std::shared_ptr<Chunk> EthernetMacHeaderSerializer::deserialize(MemoryInputStream& stream) const
{
    std::shared_ptr<EtherFrame> ethernetMacHeader = nullptr;
    MACAddress destAddr = stream.readMACAddress();
    MACAddress srcAddr = stream.readMACAddress();
    uint16_t typeOrLength = stream.readUint16();
    // detect and create the real type
    if (typeOrLength >= 0x0600 || typeOrLength == 0) {
        auto ethernetIIHeader = std::make_shared<EthernetIIFrame>();
        ethernetIIHeader->setEtherType(typeOrLength);
        ethernetMacHeader = ethernetIIHeader;
    }
    else {
        std::shared_ptr<EtherFrameWithLLC> ethernetHeaderWithLLC = nullptr;
        uint8_t ssap = stream.readByte();
        uint8_t dsap = stream.readByte();
        uint8_t ctrl = stream.readByte();
        if (dsap == 0xAA && ssap == 0xAA) { // snap frame
            auto ethSnap = std::make_shared<EtherFrameWithSNAP>();
            ethSnap->setOrgCode(((uint32_t)stream.readByte() << 16) + stream.readUint16());
            ethSnap->setLocalcode(stream.readUint16());
            ethernetHeaderWithLLC = ethSnap;
        }
        else
            ethernetHeaderWithLLC = std::make_shared<EtherFrameWithLLC>();
        ethernetHeaderWithLLC->setPayloadLength(typeOrLength);
        ethernetHeaderWithLLC->setDsap(dsap);
        ethernetHeaderWithLLC->setSsap(ssap);
        ethernetHeaderWithLLC->setControl(ctrl);
        ethernetMacHeader = ethernetHeaderWithLLC;
    }
    ethernetMacHeader->setDest(destAddr);
    ethernetMacHeader->setSrc(srcAddr);
    return ethernetMacHeader;
}

void EthernetPaddingSerializer::serialize(MemoryOutputStream& stream, const std::shared_ptr<Chunk>& chunk) const
{
    stream.writeByteRepeatedly(0, byte(chunk->getChunkLength()).get());
}

std::shared_ptr<Chunk> EthernetPaddingSerializer::deserialize(MemoryInputStream& stream) const
{
    auto ethernetPadding = std::make_shared<EthernetPadding>();
    ethernetPadding->setChunkLength(byte(stream.getLength() - stream.getPosition()));
    return ethernetPadding;
}

void EthernetFcsSerializer::serialize(MemoryOutputStream& stream, const std::shared_ptr<Chunk>& chunk) const
{
    const auto& ethernetFcs = std::static_pointer_cast<const EthernetFcs>(chunk);
    if (ethernetFcs->getFcsMode() != FCS_COMPUTED)
        throw cRuntimeError("Cannot serialize Ethernet FCS without a properly computed FCS");
    stream.writeUint32(ethernetFcs->getFcs());
}

std::shared_ptr<Chunk> EthernetFcsSerializer::deserialize(MemoryInputStream& stream) const
{
    auto ethernetFcs = std::make_shared<EthernetFcs>();
    ethernetFcs->setFcs(stream.readUint32());
    ethernetFcs->setFcsMode(FCS_COMPUTED);
    return ethernetFcs;
}

void EthernetPhyHeaderSerializer::serialize(MemoryOutputStream& stream, const std::shared_ptr<Chunk>& chunk) const
{
    stream.writeByteRepeatedly(0x55, PREAMBLE_BYTES); // preamble
    stream.writeByte(0xD5); // SFD
}

std::shared_ptr<Chunk> EthernetPhyHeaderSerializer::deserialize(MemoryInputStream& stream) const
{
    auto ethernetPhyHeader = std::make_shared<EtherPhyFrame>();
    bool preambleReadSuccessfully = stream.readByteRepeatedly(0x55, PREAMBLE_BYTES); // preamble
    uint8_t sfd = stream.readByte();
    if (!preambleReadSuccessfully || sfd != 0xD5) {
//        ethernetPhyHeader->markIncorrect();
    }
//    return ethernetPhyHeader;
    return nullptr;
}

} // namespace serializer

} // namespace inet

