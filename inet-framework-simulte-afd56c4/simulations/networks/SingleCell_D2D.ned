// 
//                           SimuLTE
// 
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself, 
// and cannot be removed from it.
// 
package lte.simulations.networks;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth10M;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.node.inet.*;
import lte.world.radio.ChannelControl;
import lte.corenetwork.binder.LteBinder;
import lte.corenetwork.nodes.Ue;
import lte.corenetwork.nodes.eNodeB;
import lte.world.radio.LteChannelControl;
import lte.epc.PgwStandardSimplified;

////////////// JOELIAS //////////////
import inet.common.lifecycle.LifecycleController;   
import inet.environment.common.PhysicalEnvironment;
import inet.node.inet.INetworkNode;
import inet.common.figures.DelegateSignalConfigurator;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.inet.INetworkNode;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;

import inet.power.base.EnergyConsumerBase;
import inet.power.base.EnergyGeneratorBase;
import inet.power.base.EnergySinkBase;
import inet.power.base.EnergySourceBase;
import inet.power.base.EnergyStorageBase;
import inet.power.consumer.AlternatingEnergyConsumer;
import inet.power.contract.*;
import inet.power.generator.*;
import inet.power.storage.*;
import inet.node.inet.NodeBase;

import inet.common.lifecycle.NodeStatus;
import inet.common.packet.PcapRecorder;
import inet.linklayer.contract.IExternalNic;
import inet.linklayer.contract.ITunNic;
import inet.linklayer.contract.IWiredNic;
import inet.linklayer.contract.IWirelessNic;
import inet.linklayer.loopback.LoopbackInterface;
import inet.mobility.contract.IMobility;
import inet.networklayer.contract.IRoutingTable;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.contract.INetworkLayer;
import inet.power.contract.IEnergyStorage;
import inet.power.contract.IEnergyGenerator;
////////////// JOELIAS //////////////


network SingleCell_D2D
{
    parameters:
        int numUeCell = default(1);
        int numUeD2DTx = default(0);
        int numUeD2DRx = default(0);
        @display("i=block/network2;bgb=991,558;bgi=background/budapest");
    submodules:
        channelControl: LteChannelControl {
            @display("p=50,25;is=s");
        }
        routingRecorder: RoutingTableRecorder {
            @display("p=50,75;is=s");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=50,125");
            config = xmldoc("demo.xml");
        }
        binder: LteBinder {
            @display("p=50,175;is=s");
        }
        server: StandardHost {
            @display("p=212,118;is=n;i=device/server");
        }
        router: Router {
            @display("p=321,136;i=device/smallrouter");
        }
        pgw: PgwStandardSimplified {
            nodeType = "PGW";
            @display("p=519,175;is=l");
        }
        eNodeB: eNodeB {
            @display("p=391,259;is=vl");
        }
        ueCell[numUeCell]: Ue {
            @display("p=292,367");
        }
        ueD2DTx[numUeD2DTx]: Ue {
            @display("p=477,367");
        }
        ueD2DRx[numUeD2DRx]: Ue {
            @display("p=544,299");
        }
        
        	///// JOELIAS /////
        lifecycleController: LifecycleController {
            @display("p=50,225");
        }
        	///// JOELIAS /////
        
    connections:
        server.pppg++ <--> Eth10G <--> router.pppg++;
        router.pppg++ <--> Eth10G <--> pgw.filterGate;
        pgw.pppg++ <--> Eth10G <--> eNodeB.ppp;
}
