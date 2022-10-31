#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include <fstream>

using namespace ns3;


NS_LOG_COMPONENT_DEFINE("Homework");

int
main(int argc, char* argv[])
{

/*
ATTENZIONE AD ATTIVARE I LOGGER, SONO MOLTO VERBOSI

*/
    //Qui l'utente può scegliere la configurazione e verbose
    // passando come parametro "configuration=configurazione prescelta(0,1,2) verbose=false,true"
    // esempio: /ns3 run homework -- configuration=1 verbose=false
    bool verbose=false;
    int configuration=2;
    CommandLine cmd;
    cmd.AddValue("configuration","Scegli Configuration",configuration);
    cmd.AddValue("configuration","Scegli verbose",verbose);
    if (verbose) {
      LogComponentEnable ("PacketSink", LOG_LEVEL_ALL);
      LogComponentEnable ("TcpL4Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("Ipv4Interface", LOG_LEVEL_ALL);
      LogComponentEnable ("OnOffApplication", LOG_LEVEL_ALL);
    }

    //
    // Allow the user to override any of the defaults and the above Bind() at
    // run-time, via command-line arguments
    //

    unsigned nCsma1=2;
    unsigned nCsma2=2;
 
    //
    // QUI VENGONO CREATI TUTTI I NODI DELLA TOPOLOGIA, LA NUMERAZIONE RIMANE COERENTE ANCHE NEL LOGGER
    //
    NS_LOG_INFO("Create nodes.");

    NodeContainer n0n1n2;
    n0n1n2.Create(nCsma1+1);

    NodeContainer n1n3;
    n1n3.Add(n0n1n2.Get(1));
    n1n3.Create(1);

    NodeContainer n4n5;
    n4n5.Create(2);

    NodeContainer n6n7n8;
    n6n7n8.Create(nCsma2+1);

    NodeContainer n5n6;
    n5n6.Add(n4n5.Get(1));
    n5n6.Add(n6n7n8.Get(0));

    NodeContainer n3n6;
    n3n6.Add(n1n3.Get(1));
    n3n6.Add(n5n6.Get(1));


    NS_LOG_INFO("Create channels.");
    //
    // QUI VENGONO CREATI I CANALI COMUNICATIVI COME DA TOPOLOGIA
    // ATTENZIONE: TUTTO QUELLO CHE CI STA È STATO COPIATO E INCOLLATO DAI FILE FORNITI DA LACAVA
    CsmaHelper csma1;
    csma1.SetChannelAttribute("DataRate", StringValue("25Mbps"));
    csma1.SetChannelAttribute("Delay", TimeValue(MicroSeconds(10)));

    CsmaHelper csma2;
    csma2.SetChannelAttribute("DataRate", StringValue("30Mbps"));
    csma2.SetChannelAttribute("Delay", TimeValue(MicroSeconds(20)));

    PointToPointHelper l0;
    l0.SetDeviceAttribute("DataRate",StringValue("80Mbps"));
    l0.SetChannelAttribute("Delay",StringValue("5us"));

    PointToPointHelper l1;
    l1.SetDeviceAttribute("DataRate",StringValue("80Mbps"));
    l1.SetChannelAttribute("Delay",StringValue("5us"));

    PointToPointHelper l2;
    l2.SetDeviceAttribute("DataRate",StringValue("80Mbps"));
    l2.SetChannelAttribute("Delay",StringValue("5us"));

    PointToPointHelper l3;
    l3.SetDeviceAttribute("DataRate",StringValue("80Mbps"));
    l3.SetChannelAttribute("Delay",StringValue("5us"));

/* I NET DEVICE CONTAINER SERVONO AD INSTALLARE I NODI SULL'INFRSTRUTTRA HARDWARE CREATA */
    NetDeviceContainer csma1Devices = csma1.Install(n0n1n2);
    NetDeviceContainer csma2Devices = csma2.Install(n6n7n8);

    NetDeviceContainer l0Devices=l0.Install(n1n3);
    NetDeviceContainer l1Devices=l1.Install(n3n6);
    NetDeviceContainer l2Devices=l2.Install(n4n5);
    NetDeviceContainer l3Devices=l3.Install(n5n6);

// QUI VIENE INSERITA LA STACK SU OGNI NODO. È ESSENZIALE CHE CIÒ AVVENGA SU TUTTI I NODI
    InternetStackHelper internet;
    internet.Install(n0n1n2);
    internet.Install(n1n3.Get(1));
    internet.Install(n4n5);
    internet.Install(n6n7n8);

    //
    // We've got the "hardware" in place.  Now we need to add IP addresses.
    // 
//p2p interfaces

    Ipv4AddressHelper address; 
    address.SetBase("10.0.1.0", "/30");
    Ipv4InterfaceContainer l0Interfaces=address.Assign(l0Devices);
    address.SetBase("10.0.2.0", "/30");
    Ipv4InterfaceContainer l1Interfaces=address.Assign(l1Devices);
    address.SetBase("10.0.3.0", "/30");
    Ipv4InterfaceContainer l2Interfaces=address.Assign(l2Devices);
    address.SetBase("10.0.4.0", "/30");
    Ipv4InterfaceContainer l3Interfaces=address.Assign(l3Devices);
    NS_LOG_INFO("Create Applications.");


//csma interfaces  

    address.SetBase("192.138.1.0", "/24");
    Ipv4InterfaceContainer csma1Interfaces=address.Assign(csma1Devices);

    address.SetBase("192.138.2.0", "/24");
    Ipv4InterfaceContainer csma2Interfaces=address.Assign(csma2Devices);
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    AsciiTraceHelper ascii;
   
 //DA QUI IN POI IL CODICE È POCO PIÙ DI UNA BOZZA OTTENUTA COPIANDO E INCOLLANDO PARTI DI CODICE VISTE A LEZIONE
 // È LA PARTE PIÙ CRITICA: NON FUNZIONA NEMMENO!  
    if (configuration==0) {
      // creazione sink su n2
      short unsigned port = 2400;
      Address sinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port));
      PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkLocalAddress);
      ApplicationContainer sinkApp = sinkHelper.Install(n0n1n2.Get(2));
      sinkApp.Start(Seconds(0.0));
      sinkApp.Stop(Seconds(20.0));

      //creazione onoffclient su n4

      OnOffHelper clientHelper("ns3::TcpSocketFactory",Address());
      clientHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
      clientHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
      AddressValue clientAddress(InetSocketAddress(csma1Interfaces.GetAddress(2), port));
      clientHelper.SetAttribute("Remote", clientAddress);
      clientHelper.SetAttribute("PacketSize", UintegerValue(1500));
      ApplicationContainer clientApp;
      clientApp.Add(clientHelper.Install(n4n5.Get(0)));
      clientApp.Start(Seconds(3.0));
      clientApp.Stop(Seconds(15.0));

      //ascii trace
      //p2p.EnableAsciiAll(ascii.CreateFileStream("tcp-star-server.tr"
      csma1.EnableAscii("task1-0-2",csma1Devices.Get(2));
      l2.EnableAscii("task1-0-4",l2Devices.Get(0));


      // PCAP NODI RICHIESTI
      l0.EnablePcap("task1-0-3",l0Devices.Get(1));
      l3.EnablePcap("task1-0-5",l3Devices.Get(0));
      l3.EnablePcap("task1-0-6",l3Devices.Get(1));
      csma2.EnablePcap("task1-0-6 prova",csma2Devices.Get(0),true);

      
    }
    else if  (configuration==1) {
      // creazione sink1 su n2
      short unsigned port1 = 2400;
      Address sinkLocalAddress1(InetSocketAddress(Ipv4Address::GetAny(), port1));
      PacketSinkHelper sinkHelper1("ns3::TcpSocketFactory", sinkLocalAddress1);
      ApplicationContainer sinkApp;
      sinkApp.Add(sinkHelper1.Install(n0n1n2.Get(2)));

      // creazione sink2 su n0
      short unsigned port2 = 7777;
      Address sinkLocalAddress2(InetSocketAddress(Ipv4Address::GetAny(), port2));
      PacketSinkHelper sinkHelper2("ns3::TcpSocketFactory", sinkLocalAddress2);
      sinkApp.Add(sinkHelper2.Install(n0n1n2.Get(0)));
      sinkApp.Start(Seconds(0.0));
      sinkApp.Stop(Seconds(20.0));

      //creazione onoffclient1 su n4
      //ATTENZIONE:PENSO CI SIA UN ERRORE NEL TESTO, DICE CHE QUESTO INVIA DATI A n1 MA SE IL SERVER
      //È SU n0 NON HA SENSO.

      OnOffHelper clientHelper1("ns3::TcpSocketFactory",Address());
      clientHelper1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
      clientHelper1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
      AddressValue clientAddress1(InetSocketAddress(csma1Interfaces.GetAddress(0), port2));
      clientHelper1.SetAttribute("Remote", clientAddress1);
      clientHelper1.SetAttribute("PacketSize", UintegerValue(2500));
      ApplicationContainer clientApp1;
      clientApp1.Add(clientHelper1.Install(n4n5.Get(0)));
      clientApp1.Start(Seconds(5.0));
      clientApp1.Stop(Seconds(15.0));

      //creazione onoffclient2 su n8

      OnOffHelper clientHelper2("ns3::TcpSocketFactory",Address());
      clientHelper2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
      clientHelper2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
      AddressValue clientAddress2(InetSocketAddress(csma1Interfaces.GetAddress(2), port1));
      clientHelper2.SetAttribute("Remote", clientAddress2);
      clientHelper2.SetAttribute("PacketSize", UintegerValue(4500));
      ApplicationContainer clientApp2;
      clientApp2.Add(clientHelper2.Install(n6n7n8.Get(2)));
      clientApp2.Start(Seconds(2.0));
      clientApp2.Stop(Seconds(9.0));

      //ascii trace
      //p2p.EnableAsciiAll(ascii.CreateFileStream("tcp-star-server.tr"
      csma1.EnableAscii("task1-1-2",csma1Devices.Get(2));
      l2.EnableAscii("task1-1-4",l2Devices.Get(0));
      csma1.EnableAscii("task1-1-0",csma1Devices.Get(0));
      csma2.EnableAscii("task1-1-8",csma2Devices.Get(2));

      // PCAP NODI RICHIESTI
      l0.EnablePcap("task1-1-3",l0Devices.Get(1));
      l3.EnablePcap("task1-1-5",l3Devices.Get(0));
      csma2.EnablePcap("task1-1-6",csma2Devices.Get(0),true);
    }
    else if  (configuration==2) {
      //UdpEchoServer su n2
      short unsigned port1=63;
      UdpEchoServerHelper echoserver(port1);
      ApplicationContainer serverApps = echoserver.Install(n0n1n2.Get(2));
      
      
      //UdpEchoClient su n8
      UdpEchoClientHelper echoclient(csma1Interfaces.GetAddress(2), port1);
      echoclient.SetAttribute("MaxPackets", UintegerValue(5));
      echoclient.SetAttribute("PacketSize", UintegerValue(2560));
      echoclient.SetAttribute("Interval", TimeValue(Seconds(2.0)));
      ApplicationContainer echoclientApp = echoclient.Install(n6n7n8.Get(2));
      echoclientApp.Start(Seconds(3.0));
      echoclientApp.Stop(Seconds(11.1));
      int sommamatricole=1933744+1945149+1960602+1900000;
      echoclient.SetFill(echoclientApp.Get(0),std::to_string(sommamatricole));

      //TCP Sink su n2
      short unsigned port2 = 2600;
      Address tcpsinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port2));
      PacketSinkHelper tcpsinkHelper("ns3::TcpSocketFactory", tcpsinkLocalAddress);
      serverApps.Add(tcpsinkHelper.Install(n0n1n2.Get(2)));
      
      //TCP onoffclient su n4 
      OnOffHelper tcpclientHelper("ns3::TcpSocketFactory",Address());
      tcpclientHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
      tcpclientHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
      AddressValue tcpclientAddress(InetSocketAddress(csma1Interfaces.GetAddress(2), port2));
      tcpclientHelper.SetAttribute("Remote", tcpclientAddress);
      tcpclientHelper.SetAttribute("PacketSize", UintegerValue(3000));
      ApplicationContainer tcpclientApp;
      tcpclientApp.Add(tcpclientHelper.Install(n4n5.Get(0)));
      tcpclientApp.Start(Seconds(3.0));
      tcpclientApp.Stop(Seconds(9.0));

      //UDP Sink su n0
      short unsigned port3 = 2500;
      Address udpsinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port3));
      PacketSinkHelper udpsinkHelper("ns3::UdpSocketFactory", udpsinkLocalAddress);
      serverApps.Add(udpsinkHelper.Install(n0n1n2.Get(0)));

      //UDP onoffclient su n7
      OnOffHelper udpclientHelper("ns3::UdpSocketFactory",Address());
      udpclientHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
      udpclientHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
      AddressValue udpclientAddress(InetSocketAddress(csma1Interfaces.GetAddress(0), port3));
      udpclientHelper.SetAttribute("Remote", udpclientAddress);
      udpclientHelper.SetAttribute("PacketSize", UintegerValue(3000));
      ApplicationContainer udpclientApp;
      udpclientApp.Add(udpclientHelper.Install(n6n7n8.Get(1)));
      udpclientApp.Start(Seconds(5.0));
      udpclientApp.Stop(Seconds(15.0));

      //starting server apps
      serverApps.Start(Seconds(0.0));
      serverApps.Stop(Seconds(20.0));


/* CATTURE DI PROVA
      csma1.EnablePcap("task1-2-2",csma1Devices.Get(2));
      csma1.EnablePcap("task1-2-0",csma1Devices.Get(0));
*/
      //ascii trace
      //p2p.EnableAsciiAll(ascii.CreateFileStream("tcp-star-server.tr"
      csma1.EnableAscii("task1-2-2",csma1Devices.Get(2));
      l2.EnableAscii("task1-2-4",l2Devices.Get(0));
      csma1.EnableAscii("task1-2-0",csma1Devices.Get(0));
      csma2.EnableAscii("task1-2-8",csma2Devices.Get(2));
      csma2.EnableAscii("task1-2-7",csma2Devices.Get(1));

      // PCAP NODI RICHIESTI 
      l0.EnablePcap("task1-2-3",l0Devices.Get(1));
      l3.EnablePcap("task1-2-5",l3Devices.Get(0));
      csma2.EnablePcap("task1-2-6",csma2Devices.Get(0),true);

    }
    else{
      perror ("configuration può assumere solo i valori interi tra 0 e 2");
      exit(EXIT_FAILURE);
    }

#if 0
//
// Users may find it convenient to initialize echo packets with actual data;
// the below lines suggest how to do this
//
  client.SetFill (apps.Get (0), "Hello World");

  client.SetFill (apps.Get (0), 0xa5, 1024);

  uint8_t fill[] = { 0, 1, 2, 3, 4, 5, 6};
  client.SetFill (apps.Get (0), fill, sizeof(fill), 1024);
#endif




    //
    // Now, do the actual simulation.
    //
    NS_LOG_INFO("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}
