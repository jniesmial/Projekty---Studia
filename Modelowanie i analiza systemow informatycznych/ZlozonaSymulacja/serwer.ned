
module Serwer
{
    gates:
        input serwerWejscie1;
        input serwerWejscie2;
        
        output serwerWyjscie1;
        output serwerWyjscie2;
     submodules:
         Procesor: Procesor{
         }
         
         Cache: Cache {
         }
         
         Dysk: Dysk{
         }
      connections:
          serwerWejscie1-->Procesor.procWejscie1;
          serwerWejscie2-->Procesor.procWejscie2;
          
          serwerWyjscie1<--Procesor.procWyjscie1;
          serwerWyjscie2<--Procesor.procWyjscie2;
          
          Procesor.procWeWy<-->Cache.cacheWeWy1;
          Cache.cacheWeWy2<-->Dysk.dyskWeWy;
}
