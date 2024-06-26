//
// Copyright (C) 2014 Florian Meier
// Copyright (C) 2013 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef __INET_IEEE802154NARROWBANDRECEIVER_H
#define __INET_IEEE802154NARROWBANDRECEIVER_H

#include "inet/physicallayer/wireless/common/base/packetlevel/FlatReceiverBase.h"

namespace inet {

namespace physicallayer {

class INET_API Ieee802154NarrowbandReceiver : public FlatReceiverBase
{
  protected:
    W minInterferencePower;

  public:
    Ieee802154NarrowbandReceiver();

    void initialize(int stage) override;

    virtual bool computeIsReceptionPossible(const IListening *listening, const ITransmission *transmission) const override;
    virtual bool computeIsReceptionPossible(const IListening *listening, const IReception *reception, IRadioSignal::SignalPart part) const override;

    virtual std::ostream& printToStream(std::ostream& stream, int level, int evFlags = 0) const override;

    virtual W getMinInterferencePower() const override { return minInterferencePower; }
};

} // namespace physicallayer

} // namespace inet

#endif

