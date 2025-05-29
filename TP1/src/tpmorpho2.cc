#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Image.hh"
#include "morfo.hh"
#include "tpmorpho1.hh"
#include "tpmorpho2.hh"


// Cette fonction prend une image en paramètre et la modifie
void TP2_Version1(Image<int>& p,int seuil,int parametre) {
  int i,j,index;
  int* PIX=p.PI();
  int H=p.PL();
  int W=p.PW();
  
   index = 0;
   for (j=0;j<H;j++)
    for (i=0;i<W;i++) {
      if ((i<=parametre)) {
	if (PIX[index]>=seuil)
	  PIX[index]=255;
	else PIX[index]=0;
       }
      index++;
     }
}

// Cette fonction prend 2 images en paramètres et modifie la première
void TP2_Version2(Image<int>& p1,Image<int>& p2,int seuil,int parametre) {
  int index=0;
  int* PIX=p1.PI();
  int H=p1.PL();
  int W=p1.PW();
  int i,j;
  
  for (j=0;j<H;j++)
    for (i=0;i<W;i++) {
      if (p1.X(i,j) > p2.X(i+parametre,j) + seuil)
	   PIX[index]=255;
	 else PIX[index]=0;
      index++;
    }
}

// Cette fonction ne modifie pas d'image et renvoie un tableau T en sortie
void TP2_Version3(Image<int>& p,int seuil,int parametre,float T[3]) {
  int i,index;
  int* PIX=p.PI();
  int H=p.PL();
  int W=p.PW();
  int *C = (int*)calloc(3,sizeof(int));

  for (index=0;index<W*H;index++) {
     if (PIX[index] < seuil-parametre)
	  C[0]++;
     else if (PIX[index] > seuil+parametre)
          C[2]++;
     else C[1]++;
   }
  for (i=0;i<3;i++) T[i] = 100.0*((float)C[i])/(W*H);
  free(C);
}

