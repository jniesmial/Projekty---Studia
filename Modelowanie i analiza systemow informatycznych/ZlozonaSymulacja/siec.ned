
network Siec
{
    submodules:
        Klient1: Klient{}
        Klient2: Klient{}
        
        KartaSieciowa1: KartaSieciowa{}
        KartaSieciowa2: KartaSieciowa{}
        KartaSieciowa3: KartaSieciowa{}
        KartaSieciowa4: KartaSieciowa{}
        
        Serwer: Serwer{}
        
        ModulStatystyk: ModStat{}
     
     connections:
         Klient1.klientWyjscie-->KartaSieciowa1.KSwejscie;
         Klient2.klientWyjscie-->KartaSieciowa2.KSwejscie;
         
         KartaSieciowa1.KSwyjscie-->{datarate= 100Mbps;}-->Serwer.serwerWejscie1;
         KartaSieciowa2.KSwyjscie-->{datarate= 100Mbps;}-->Serwer.serwerWejscie2;
         
         Serwer.serwerWyjscie1-->KartaSieciowa3.KSwejscie;
         Serwer.serwerWyjscie2-->KartaSieciowa4.KSwejscie;
         
         KartaSieciowa3.KSwyjscie-->{datarate= 100Mbps;}-->ModulStatystyk.modStatWejscie1;
         KartaSieciowa4.KSwyjscie-->{datarate= 100Mbps;}-->ModulStatystyk.modStatWejscie2;
}
