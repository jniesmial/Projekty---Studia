
module Komputer
{
    @display("bgb=351,230");
 	 gates:
        input wejscie;
        output wyjscie; 
 	submodules:
 		CPU: Kolejka {
 	parameters:
 	@display("p=269,91;i=device/cpu");
 	}
 		ssd: Kolejka {

	 @display("p=74,91;i=device/disk");
 	}
 		
 	connections:
 		wejscie --> CPU.wejscie;
        CPU.wyjscie --> ssd.wejscie;
        ssd.wyjscie --> wyjscie;
}
