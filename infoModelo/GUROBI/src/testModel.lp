\ LP format - for model browsing. Use MPS format to capture full model detail.
\ Signature: 0xecb3a4c5f7b71659
Minimize
  100000 x(0,0,0) + 100000 x(0,0,1) + 100000 x(0,0,2) + 100000 x(0,0,3)
   + 100000 x(0,1,0) + 100000 x(0,1,1) + 100000 x(0,1,2) + 100000 x(0,1,3)
   + 100000 x(0,2,0) + 100000 x(0,2,1) + 100000 x(0,2,2) + 100000 x(0,2,3)
   + 100000 x(1,0,0) + 100000 x(1,0,1) + 100000 x(1,0,2) + 100000 x(1,0,3)
   + 100000 x(1,1,0) + 100000 x(1,1,1) + 100000 x(1,1,2) + 100000 x(1,1,3)
   + 100000 x(1,2,0) + 100000 x(1,2,1) + 100000 x(1,2,2) + 100000 x(1,2,3)
Subject To
 RestricaoFixaPrimeiroElemento0: x(0,0,0) + x(0,0,1) + x(0,0,2) + x(0,0,3)
   + x(0,1,0) + x(0,1,1) + x(0,1,2) + x(0,1,3) + x(0,2,0) + x(0,2,1)
   + x(0,2,2) + x(0,2,3) >= 1
 RestricaoFixaPrimeiroElemento1: x(1,0,0) + x(1,0,1) + x(1,0,2) + x(1,0,3)
   + x(1,1,0) + x(1,1,1) + x(1,1,2) + x(1,1,3) + x(1,2,0) + x(1,2,1)
   + x(1,2,2) + x(1,2,3) >= 1
 RestricaoFixaSegundoElemento0: x(0,0,0) + x(0,0,1) + x(0,0,2) + x(0,0,3)
   + x(1,0,0) + x(1,0,1) + x(1,0,2) + x(1,0,3) = 1
 RestricaoFixaSegundoElemento1: x(0,1,0) + x(0,1,1) + x(0,1,2) + x(0,1,3)
   + x(1,1,0) + x(1,1,1) + x(1,1,2) + x(1,1,3) = 1
 RestricaoFixaSegundoElemento2: x(0,2,0) + x(0,2,1) + x(0,2,2) + x(0,2,3)
   + x(1,2,0) + x(1,2,1) + x(1,2,2) + x(1,2,3) = 1
 RestricaoFixaTerceiroElemento0: x(0,0,0) + x(0,1,0) + x(0,2,0) + x(1,0,0)
   + x(1,1,0) + x(1,2,0) >= 1
 RestricaoFixaTerceiroElemento1: x(0,0,1) + x(0,1,1) + x(0,2,1) + x(1,0,1)
   + x(1,1,1) + x(1,2,1) >= 1
 RestricaoFixaTerceiroElemento2: x(0,0,2) + x(0,1,2) + x(0,2,2) + x(1,0,2)
   + x(1,1,2) + x(1,2,2) >= 1
 RestricaoFixaTerceiroElemento3: x(0,0,3) + x(0,1,3) + x(0,2,3) + x(1,0,3)
   + x(1,1,3) + x(1,2,3) >= 1
Bounds
Binaries
 x(0,0,0) x(0,0,1) x(0,0,2) x(0,0,3) x(0,1,0) x(0,1,1) x(0,1,2) x(0,1,3)
 x(0,2,0) x(0,2,1) x(0,2,2) x(0,2,3) x(1,0,0) x(1,0,1) x(1,0,2) x(1,0,3)
 x(1,1,0) x(1,1,1) x(1,1,2) x(1,1,3) x(1,2,0) x(1,2,1) x(1,2,2) x(1,2,3)
End
