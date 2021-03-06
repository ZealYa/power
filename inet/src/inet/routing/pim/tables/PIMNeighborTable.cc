//
// Copyright (C) 2013 Brno University of Technology (http://nes.fit.vutbr.cz/ansa)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
// Authors: Veronika Rybova, Vladimir Vesely (ivesely@fit.vutbr.cz),
//          Tamas Borbely (tomi@omnetpp.org)

#include <algorithm>

#include "inet/routing/pim/tables/PIMNeighborTable.h"

namespace inet {
Register_Abstract_Class(PIMNeighbor);
Define_Module(PIMNeighborTable);

using namespace std;

PIMNeighbor::PIMNeighbor(InterfaceEntry *ie, IPv4Address address, int version)
    : nt(nullptr), ie(ie), address(address), version(version), generationId(0), drPriority(-1L)
{
    ASSERT(ie);

    livenessTimer = new cMessage("NeighborLivenessTimer", PIMNeighborTable::NeighborLivenessTimer);
    livenessTimer->setContextPointer(this);
}

PIMNeighbor::~PIMNeighbor()
{
    ASSERT(!livenessTimer->isScheduled());
    delete livenessTimer;
}

// for WATCH_MAP()
std::ostream& operator<<(std::ostream& os, const PIMNeighborTable::PIMNeighborVector& v)
{
    for (unsigned int i = 0; i < v.size(); i++) {
        PIMNeighbor *e = v[i];
        os << "[" << i << "]: "
           << "{ If = " << e->getInterfacePtr()->getName() << "; Addr = " << e->getAddress() << "; Ver = " << e->getVersion()
           << "; GenID = " << e->getGenerationId() << "; Pr = " << e->getDRPriority() << "}; ";
    }
    return os;
};

std::string PIMNeighbor::info() const
{
    std::stringstream out;
    out << "PIMNeighbor addr=" << address << ", iface=" << ie->getName() << ", v=" << version << ", priority=" << this->drPriority << "}";
    return out.str();
}

void PIMNeighbor::changed()
{
    if (nt)
        nt->emit(NF_PIM_NEIGHBOR_CHANGED, this);
}

PIMNeighborTable::~PIMNeighborTable()
{
    for (auto & elem : neighbors) {
        for (auto & _it2 : elem.second) {
            cancelEvent((_it2)->getLivenessTimer());
            delete (_it2);
        }
    }
}

void PIMNeighborTable::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        WATCH_MAP(neighbors);
    }
}

void PIMNeighborTable::handleMessage(cMessage *msg)
{
    // self message (timer)
    if (msg->isSelfMessage()) {
        if (msg->getKind() == NeighborLivenessTimer) {
            processLivenessTimer(msg);
        }
        else
            ASSERT(false);
    }
    else
        throw cRuntimeError("PIMNeighborTable received a message although it does not have gates.");
}

void PIMNeighborTable::processLivenessTimer(cMessage *livenessTimer)
{
    EV << "PIMNeighborTable::processNLTimer\n";
    PIMNeighbor *neighbor = check_and_cast<PIMNeighbor *>((cObject *)livenessTimer->getContextPointer());
    IPv4Address neighborAddress = neighbor->getAddress();
    deleteNeighbor(neighbor);
    EV << "PIM::processNLTimer: Neighbor " << neighborAddress << "was removed from PIM neighbor table.\n";
}

bool PIMNeighborTable::addNeighbor(PIMNeighbor *entry, double holdTime)
{
    Enter_Method_Silent();

    PIMNeighborVector& neighborsOnInterface = neighbors[entry->getInterfaceId()];

    for (auto & elem : neighborsOnInterface)
        if ((elem)->getAddress() == entry->getAddress())
            return false;


    EV_DETAIL << "Added new neighbor to table: " << entry->info() << "\n";
    entry->nt = this;
    neighborsOnInterface.push_back(entry);
    take(entry->getLivenessTimer());
    restartLivenessTimer(entry, holdTime);

    emit(NF_PIM_NEIGHBOR_ADDED, entry);

    return true;
}

bool PIMNeighborTable::deleteNeighbor(PIMNeighbor *neighbor)
{
    Enter_Method_Silent();

    auto it = neighbors.find(neighbor->getInterfaceId());
    if (it != neighbors.end()) {
        PIMNeighborVector& neighborsOnInterface = it->second;
        auto it2 = find(neighborsOnInterface.begin(), neighborsOnInterface.end(), neighbor);
        if (it2 != neighborsOnInterface.end()) {
            neighborsOnInterface.erase(it2);

            emit(NF_PIM_NEIGHBOR_DELETED, neighbor);

            neighbor->nt = nullptr;
            cancelEvent(neighbor->getLivenessTimer());
            delete neighbor;

            return true;
        }
    }
    return false;
}

void PIMNeighborTable::restartLivenessTimer(PIMNeighbor *neighbor, double holdTime)
{
    Enter_Method_Silent();
    cancelEvent(neighbor->getLivenessTimer());
    scheduleAt(simTime() + holdTime, neighbor->getLivenessTimer());
}

PIMNeighbor *PIMNeighborTable::findNeighbor(int interfaceId, IPv4Address addr)
{
    auto neighborsOnInterface = neighbors.find(interfaceId);
    if (neighborsOnInterface != neighbors.end()) {
        for (auto & elem : neighborsOnInterface->second)
            if ((elem)->getAddress() == addr && (elem)->getInterfaceId() == interfaceId)
                return elem;

    }
    return nullptr;
}

int PIMNeighborTable::getNumNeighbors(int interfaceId)
{
    auto it = neighbors.find(interfaceId);
    return it != neighbors.end() ? it->second.size() : 0;
}

PIMNeighbor *PIMNeighborTable::getNeighbor(int interfaceId, int index)
{
    auto it = neighbors.find(interfaceId);
    return it != neighbors.end() ? it->second.at(index) : nullptr;
}
}    // namespace inet

