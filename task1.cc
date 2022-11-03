#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include <fstream>

// Default Network Topology
//                       _ n3 _                 
//                     /        \
//                 __ /          \____________ 
//             l0                              l1
//         10.0.1.0/31                     10.0.2.0/31   
//           /                                     \____________
//     n0   n1   n2                                _________----n6   n7   n8
//     |    |    |                                l3             |    |    |
//     ===========                             10.0.4.0/31       =========== 
// CSMA1 192.138.1.0/24                         /          CSMA2 192.138.1.0/24
//     c: 25Mbps                              n5               c: 30Mbps
//     d: 10us                               /                 d: 20us
//                                         l2
//                                     10.0.3.0/31
//                                       /
//                                     n4
//     l{0, 1, 2, 3} link
//     c: 80Mbps
//     d: 5us

using namespace ns3;

// Per distinguere il vostro compito, definire la seguente riga nel file task1.cc dopo l’inclusione dei moduli e del namespace:
//     • NS_LOG_COMPONENT_DEFINE(“Task_1_Team_<numero_team>”);

NS_LOG_COMPONENT_DEFINE("Task_1_Team_34");

int 
main(int argc, char* argv[]) 
{

    //Qui l'utente può scegliere la configurazione e verbose
    //esempio: ./ns3 run "scratch/homework --configuration=0 --verbose=true"
    bool verbose=true;

    int configuration=0;

    CommandLine cmd;
    cmd.AddValue("configuration","Numero della configurazione {0, 1, 2}",configuration);
    cmd.AddValue("verbose","Tell echo applications to log if true",verbose);

    cmd.Parse(argc,argv);
    
    if (verbose) 
    {
        LogComponentEnable ("PacketSink",         LOG_LEVEL_INFO);
        LogComponentEnable ("TcpL4Protocol",      LOG_LEVEL_INFO);
        LogComponentEnable ("Ipv4Interface",      LOG_LEVEL_INFO);
        LogComponentEnable ("OnOffApplication",   LOG_LEVEL_INFO);
    }


    NS_LOG_INFO("Create nodes");

    NodeContainer n0n1n2;
    n0n1n2.Create(3);

    NodeContainer n1n3;
    n1n3.Add(n0n1n2.Get(1));
    n1n3.Create(1);

    NodeContainer n4n5;
    n4n5.Create(2);

    NodeContainer n6n7n8;
    n6n7n8.Create(3);

    NodeContainer n5n6;
    n5n6.Add(n4n5.Get(1));
    n5n6.Add(n6n7n8.Get(0));

    NodeContainer n3n6;
    n3n6.Add(n1n3.Get(1));
    n3n6.Add(n5n6.Get(1));


    NS_LOG_INFO("Create channels");

    CsmaHelper csma1;
    csma1.SetChannelAttribute("DataRate", StringValue("25Mbps"));
    csma1.SetChannelAttribute("Delay", TimeValue(MicroSeconds(10)));

    CsmaHelper csma2;
    csma2.SetChannelAttribute("DataRate", StringValue("30Mbps"));
    csma2.SetChannelAttribute("Delay", TimeValue(MicroSeconds(20)));

    PointToPointHelper l0;
    l0.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    l0.SetChannelAttribute("Delay", StringValue("5us"));

    PointToPointHelper l1;
    l1.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    l1.SetChannelAttribute("Delay", StringValue("5us"));

    PointToPointHelper l2;
    l2.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    l2.SetChannelAttribute("Delay", StringValue("5us"));

    PointToPointHelper l3;
    l3.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    l3.SetChannelAttribute("Delay", StringValue("5us"));

    // I NetDeviceContainer servono ad installare i nodi sull'infrastuttura Hardware creata
    NetDeviceContainer csma1Devices = csma1.Install(n0n1n2);
    NetDeviceContainer csma2Devices = csma2.Install(n6n7n8);

    NetDeviceContainer l0Devices=l0.Install(n1n3);
    NetDeviceContainer l1Devices=l1.Install(n3n6);
    NetDeviceContainer l2Devices=l2.Install(n4n5);
    NetDeviceContainer l3Devices=l3.Install(n5n6);


    // Qui viene inserita la protocol stack su ogni nodo. È essenziale che ciò avvenga su tutti i nodi
    //!! error: no matching member function for call to 'Install' , mi dà questo errore quando compilo, perchè?! :(( -Andrea
    InternetStackHelper internet;
    internet.Install(n0n1n2);
    internet.Install(n1n3.Get(1));
    internet.Install(n4n5);
    internet.Install(n6n7n8);

    // We've got the "hardware" in place.  Now we need to add IP addresses.
    // p2p interfaces
    Ipv4AddressHelper address; 

    address.SetBase("10.0.1.0", "/30");
    Ipv4InterfaceContainer l0Interfaces = address.Assign(l0Devices);

    address.SetBase("10.0.2.0", "/30");
    Ipv4InterfaceContainer l1Interfaces = address.Assign(l1Devices);

    address.SetBase("10.0.3.0", "/30");
    Ipv4InterfaceContainer l2Interfaces = address.Assign(l2Devices);

    address.SetBase("10.0.4.0", "/30");
    Ipv4InterfaceContainer l3Interfaces = address.Assign(l3Devices);
    
    // csma interfaces
    address.SetBase("192.138.1.0", "/24");
    Ipv4InterfaceContainer csma1Interfaces = address.Assign(csma1Devices);

    address.SetBase("192.138.2.0", "/24");
    Ipv4InterfaceContainer csma2Interfaces = address.Assign(csma2Devices);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();   //?????
    
    NS_LOG_INFO("Create Applications.");    



    // Per tutte le configurazioni:
    //     o 20s per ogni run
    //     o Abilitare il packet tracing PCAP solo nei nodi n3, n5, n6
    //     o Abilitare ASCII tracing solo sui client ed i server
    AsciiTraceHelper ascii;
    //     o Il nome per le tracce da generare deve rispettare il seguente formato:
    //          task1-<configuration>-<id_del_nodo>.<formato_file_richiesto(.pcap|.tr)>

    // • configuration 0:
    if (configuration == 0) 
    {    
        //  o TCP Sink su
        //       n2, porta 2400
        short unsigned port = 2400;
        Address sinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port));
        PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkLocalAddress);
        ApplicationContainer sinkApp = sinkHelper.Install(n0n1n2.Get(2));
        sinkApp.Start(Seconds(0.0));
        sinkApp.Stop(Seconds(20.0));

        //  o TCP OnOff Client n4
        OnOffHelper clientHelper("ns3::TcpSocketFactory",Address());
        clientHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        clientHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        AddressValue clientAddress(InetSocketAddress(csma1Interfaces.GetAddress(2), port));
        clientHelper.SetAttribute("Remote", clientAddress);

        //       Packet size: 1500 bytes
        clientHelper.SetAttribute("PacketSize", UintegerValue(1500));

        ApplicationContainer clientApp;
        clientApp.Add(clientHelper.Install(n4n5.Get(0)));
        //       Inizio invio dati:3s
        clientApp.Start(Seconds(3.0));
        //       Fine invio dati: 15s
        clientApp.Stop(Seconds(15.0));
        
        // ascii trace
        // p2p.EnableAsciiAll(ascii.CreateFileStream("tcp-star-server.tr"
        csma1.EnableAscii("task1-0-n2",csma1Devices.Get(2));
        l2.EnableAscii("task1-0-4",l2Devices.Get(0));


        // PCAP NODI RICHIESTI
        l0.EnablePcap("task1-0-n3",l0Devices.Get(1));
        l3.EnablePcap("task1-0-n5",l3Devices.Get(0));
        l3.EnablePcap("task1-0-n6",l3Devices.Get(1));
        csma2.EnablePcap("task1-0-n6 prova",csma2Devices.Get(0),true);

    }

    else if (configuration == 1)
    {
        // o TCP Sink su
        //      n2, porta 2400
        short unsigned port1 = 2400;
        Address sinkLocalAddress1(InetSocketAddress(Ipv4Address::GetAny(), port1));
        PacketSinkHelper sinkHelper1("ns3::TcpSocketFactory", sinkLocalAddress1);
        ApplicationContainer sinkApp;
        sinkApp.Add(sinkHelper1.Install(n0n1n2.Get(2)));

        //      n0, porta 7777
        short unsigned port2 = 7777;
        Address sinkLocalAddress2(InetSocketAddress(Ipv4Address::GetAny(), port2));
        PacketSinkHelper sinkHelper2("ns3::TcpSocketFactory", sinkLocalAddress2);
        sinkApp.Add(sinkHelper2.Install(n0n1n2.Get(0)));
        sinkApp.Start(Seconds(0.0));
        sinkApp.Stop(Seconds(20.0));
        
        // o TCP OnOff Client n4 che manda dati a n0

        OnOffHelper clientHelper1("ns3::TcpSocketFactory",Address());
        clientHelper1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        clientHelper1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        AddressValue clientAddress1(InetSocketAddress(csma1Interfaces.GetAddress(0), port2));
        clientHelper1.SetAttribute("Remote", clientAddress1);
        //      Packet size: 2500 bytes
        clientHelper1.SetAttribute("PacketSize", UintegerValue(2500));

        ApplicationContainer clientApp1;
        clientApp1.Add(clientHelper1.Install(n4n5.Get(0)));
        //      Inizio invio dati: 5s
        clientApp1.Start(Seconds(5.0));
        //      Fine invio dati: 15s
        clientApp1.Stop(Seconds(15.0));
        
        // o TCP OnOff Client n8 che manda dati a n2 
        OnOffHelper clientHelper2("ns3::TcpSocketFactory",Address());
        clientHelper2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        clientHelper2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        AddressValue clientAddress2(InetSocketAddress(csma1Interfaces.GetAddress(2), port1));
        clientHelper2.SetAttribute("Remote", clientAddress2);
        //      Packet size: 4500 bytes
        clientHelper2.SetAttribute("PacketSize", UintegerValue(4500));

        ApplicationContainer clientApp2;
        clientApp2.Add(clientHelper2.Install(n6n7n8.Get(2)));
        //      Inizio invio dati: 2s
        clientApp2.Start(Seconds(2.0));
        //      Fine invio dati: 9s
        clientApp2.Stop(Seconds(9.0));
        
        // ascii trace
        // p2p.EnableAsciiAll(ascii.CreateFileStream("tcp-star-server.tr"
        csma1.EnableAscii("task1-1-n2",csma1Devices.Get(2));
        l2.EnableAscii("task1-1-n4",l2Devices.Get(0));
        csma1.EnableAscii("task1-1-n0",csma1Devices.Get(0));
        csma2.EnableAscii("task1-1-n8",csma2Devices.Get(2));

        // PCAP NODI RICHIESTI
        l0.EnablePcap("task1-1-n3",l0Devices.Get(1));
        l3.EnablePcap("task1-1-n5",l3Devices.Get(0));
        csma2.EnablePcap("task1-1-n6",csma2Devices.Get(0),true);
    }

    else if (configuration == 2)
    {
        // o UDP Echo Server su n2, porta 63
        short unsigned port1 = 63;
        UdpEchoServerHelper echoserver(port1);
        ApplicationContainer serverApps = echoserver.Install(n0n1n2.Get(2));

        // o UDP Echo Client n8
        UdpEchoClientHelper echoclient(csma1Interfaces.GetAddress(2), port1);
        echoclient.SetAttribute("MaxPackets", UintegerValue(5));
        //      Invia 5 paccheti a 3s, 5s, 7s, 9s, 11
        echoclient.SetAttribute("Interval", TimeValue(Seconds(2.0)));
        //      Packet size: 2560 bytes
        echoclient.SetAttribute("PacketSize", UintegerValue(2560));

        ApplicationContainer echoclientApp = echoclient.Install(n6n7n8.Get(2));
        echoclientApp.Start(Seconds(3.0));
        echoclientApp.Stop(Seconds(11.1));
        
        //      Testo da inserire: somma delle vostre matricole come intero
        int sommamatricole = 1933744 + 1945149 + 1960602 + 1943362;
        //pacchetto riempito con sommamatricole seguita da spazi (prof non ha specificato come vada riempito il resto...) -sirAlex
        int numcifre = 0;
        char fill[2559];
        for (int i = 0; i < 2559; i++) fill[i] = ' '; //padding
        for (int i = sommamatricole; i > 0; i /= 10) numcifre++;
        for (int i = 0; i < numcifre; i++) {
            fill[numcifre-i-1] = (sommamatricole % 10) + '0';
            sommamatricole /= 10;
        }
        echoclient.SetFill(echoclientApp.Get(0),fill);
        
        // o TCP Sink su
        //      n2, porta 2600
        short unsigned port2 = 2600;
        Address tcpsinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port2));
        PacketSinkHelper tcpsinkHelper("ns3::TcpSocketFactory", tcpsinkLocalAddress);
        serverApps.Add(tcpsinkHelper.Install(n0n1n2.Get(2)));
      
        // o UDP Sink su
        //      n0, porta 2500
        short unsigned port3 = 2500;
        Address udpsinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port3));
        PacketSinkHelper udpsinkHelper("ns3::UdpSocketFactory", udpsinkLocalAddress);
        serverApps.Add(udpsinkHelper.Install(n0n1n2.Get(0)));

        // o TCP OnOff Client n4
        OnOffHelper tcpclientHelper("ns3::TcpSocketFactory",Address());
        tcpclientHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        tcpclientHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        AddressValue tcpclientAddress(InetSocketAddress(csma1Interfaces.GetAddress(2), port2));
        tcpclientHelper.SetAttribute("Remote", tcpclientAddress);
        //      Packet size: 3000 bytes
        tcpclientHelper.SetAttribute("PacketSize", UintegerValue(3000));

        ApplicationContainer tcpclientApp;
        tcpclientApp.Add(tcpclientHelper.Install(n4n5.Get(0)));
        //      Inizio invio dati:3s
        tcpclientApp.Start(Seconds(3.0));
        //      Fine invio dati: 9s
        tcpclientApp.Stop(Seconds(9.0));
        

        // o UDP OnOff Client n7
        OnOffHelper udpclientHelper("ns3::UdpSocketFactory",Address());
        udpclientHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        udpclientHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        AddressValue udpclientAddress(InetSocketAddress(csma1Interfaces.GetAddress(0), port3));
        udpclientHelper.SetAttribute("Remote", udpclientAddress);
        //      Packet size: 3000 bytes
        udpclientHelper.SetAttribute("PacketSize", UintegerValue(3000));

        ApplicationContainer udpclientApp;
        udpclientApp.Add(udpclientHelper.Install(n6n7n8.Get(1)));
        //      Inizio invio dati: 5s
        udpclientApp.Start(Seconds(5.0));
        //      Fine invio dati: 15s
        udpclientApp.Stop(Seconds(15.0));

        /* CATTURE DI PROVA
        csma1.EnablePcap("task1-2-2",csma1Devices.Get(2));
        csma1.EnablePcap("task1-2-0",csma1Devices.Get(0));
        */

        //ascii trace
        //p2p.EnableAsciiAll(ascii.CreateFileStream("tcp-star-server.tr"
        csma1.EnableAscii("task1-2-n2",csma1Devices.Get(2));
        l2.EnableAscii("task1-2-n4",l2Devices.Get(0));
        csma1.EnableAscii("task1-2-n0",csma1Devices.Get(0));
        csma2.EnableAscii("task1-2-n8",csma2Devices.Get(2));
        csma2.EnableAscii("task1-2-n7",csma2Devices.Get(1));

        // PCAP NODI RICHIESTI 
        l0.EnablePcap("task1-2-n3",l0Devices.Get(1));
        l3.EnablePcap("task1-2-n5",l3Devices.Get(0));
        csma2.EnablePcap("task1-2-n6",csma2Devices.Get(0),true);        
    }
    
    else
    {
        perror ("configuration può assumere solo i valori interi tra 0 e 2.");
        exit(EXIT_FAILURE);
    }

    // Now, do the actual simulation.
    NS_LOG_INFO("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;

}
