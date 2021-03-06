#include "AlternatingEnergyConsumerSimuLTE.h"

namespace inet {

namespace power {

Define_Module(AlternatingEnergyConsumerSimuLTE);

void AlternatingEnergyConsumerSimuLTE::updatePowerConsumption(){
    //if(state.get() == HostState::ACTIVE){
    if(isTransmitting==true){
        powerConsumption = isSleeping ? (W(0)+W(par("transmittingPowerConsumption"))) : (W(par("powerConsumption"))+W(par("transmittingPowerConsumption")));
    }else{
        powerConsumption = isSleeping ? W(0) : W(par("powerConsumption"));
    }

    energySource->setPowerConsumption(energyConsumerId, powerConsumption);
    emit(IEnergySource::powerConsumptionChangedSignal, powerConsumption.get());
}

} // namespace power

} // namespace inet
