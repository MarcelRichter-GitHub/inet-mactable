//
// Copyright (C) 2013 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __INET_DIMENSIONALRECEIVERANALOGMODEL_H
#define __INET_DIMENSIONALRECEIVERANALOGMODEL_H

#include "inet/physicallayer/wireless/common/analogmodel/dimensional/DimensionalReceptionAnalogModel.h"
#include "inet/physicallayer/wireless/common/base/packetlevel/NarrowbandNoiseBase.h"
#include "inet/physicallayer/wireless/common/base/packetlevel/ReceiverAnalogModelBase.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/IRadioMedium.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/IReceiverAnalogModel.h"
#include "inet/physicallayer/wireless/common/radio/packetlevel/BandListening.h"
#include "inet/physicallayer/wireless/common/radio/packetlevel/ListeningDecision.h"

namespace inet {

namespace physicallayer {

class INET_API DimensionalReceiverAnalogModel : public ReceiverAnalogModelBase, public IReceiverAnalogModel
{
  protected:
    Hz centerFrequency = Hz(NaN);
    Hz bandwidth = Hz(NaN);
    W energyDetection = W(NaN);
    W sensitivity = W(NaN);

  protected:
    virtual void initialize(int stage) override;

  public:
    virtual IListening *createListening(const IRadio *radio, const simtime_t startTime, const simtime_t endTime, const Coord& startPosition, const Coord& endPosition) const override;

    virtual const IListeningDecision *computeListeningDecision(const IListening *listening, const IInterference *interference) const override;

    virtual bool computeIsReceptionPossible(const IListening *listening, const IReception *reception, IRadioSignal::SignalPart part) const override;
    virtual bool computeIsReceptionPossible(const IListening *listening, const IReception *reception, IRadioSignal::SignalPart part, const IInterference *interference, const ISnir *snir) const override;
};

} // namespace physicallayer

} // namespace inet

#endif

