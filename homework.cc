

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

  LogComponentEnable ("PacketSink", LOG_LEVEL_ALL);
  LogComponentEnable ("TcpL4Protocol", LOG_LEVEL_ALL);
  LogComponentEnable ("OnOffApplication", LOG_LEVEL_ALL);
  */
    //
    // Allow the user to override any of the defaults and the above Bind() at
    // run-time, via command-line arguments
    //
  //  bool verbose=true;

    unsigned nCsma1=2;
    unsigned nCsma2=2;
    /*
    if (verbose) {
        LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_ALL);
        LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_ALL);
    }
    */
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
//csma interfaces  

    Ipv4AddressHelper address; 
    address.SetBase("192.138.1.0", "/24");
    Ipv4InterfaceContainer csma1Interfaces=address.Assign(csma1Devices);

    address.SetBase("192.138.2.0", "/24");
    Ipv4InterfaceContainer csma2Interfaces=address.Assign(csma2Devices);
    
//p2p interfaces

     
    address.SetBase("10.0.1.0", "/30");
    Ipv4InterfaceContainer l0Interfaces=address.Assign(l0Devices);
    address.SetBase("10.0.2.0", "/30");
    Ipv4InterfaceContainer l1Interfaces=address.Assign(l1Devices);
    address.SetBase("10.0.3.0", "/30");
    Ipv4InterfaceContainer l2Interfaces=address.Assign(l2Devices);
    address.SetBase("10.0.4.0", "/30");
    Ipv4InterfaceContainer l3Interfaces=address.Assign(l3Devices);
    NS_LOG_INFO("Create Applications.");
    
   
 //DA QUI IN POI IL CODICE È POCO PIÙ DI UNA BOZZA OTTENUTA COPIANDO E INCOLLANDO PARTI DI CODICE VISTE A LEZIONE
 // È LA PARTE PIÙ CRITICA: NON FUNZIONA NEMMENO!  
    int configuration=0;
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
      AddressValue clientAddress(InetSocketAddress(l2Interfaces.GetAddress(0), port));
      clientHelper.SetAttribute("Remote", clientAddress);
      clientHelper.SetAttribute("PacketSize", UintegerValue(1500));
      ApplicationContainer clientApp;
      clientApp.Add(clientHelper.Install(n4n5.Get(0)));
      clientApp.Start(Seconds(3.0));
      clientApp.Start(Seconds(15.0));
    }
    else if  (configuration==1) {
      exit(0);
    }
    else if  (configuration==2) {
      exit(0);
    }
    else {
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

  //  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

// HO ATTIVATO UN PO' DI PCAP, MA TUTTE LE CATTURE SONO VUOTE :(
  //???  Ipv4GlobalRoutingHelper::PopulateRoutingTables();


    l1.EnablePcap("l1",n3n6,true);
    l3.EnablePcap("l3",n5n6,true);


    csma1.EnablePcapAll("csma1",true);
    csma2.EnablePcap("csma2",csma2Devices.Get(0),true);

    //
    // Now, do the actual simulation.
    //
    NS_LOG_INFO("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}
