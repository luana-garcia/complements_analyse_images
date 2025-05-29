#include "Image.hh"
#include "morfo.hh"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int min(int a,int b) {if (a<b) return a; else return b;}
static int max(int a,int b) {if (a<b) return b; else return a;}
 
void Morphobase(Image<int>& p,char *nom,int tableau[NMAX][NMAX]) {

int x1=NMAX,x2=0,y1=NMAX,y2=0;

 for (int i=0;i<NMAX;i++)
     for (int j=0;j<NMAX;j++)
          if (tableau[i][j]) {
                if (i<x1) x1=i;if (j<y1) y1=j;
                if (i>x2) x2=i;if (j>y2) y2=j;
                }

if (strcmp(nom,"erode")==0) Appliq_morpho(p,0,-1,tableau,x1,y1,x2,y2);
else
if (strcmp(nom,"dilate")==0) Appliq_morpho(p,1,-1,tableau,x1,y1,x2,y2);
}

void Appliq_morpho(Image<int>& p,int erodil,int sens,int tab[NMAX][NMAX],int xdeb,int ydeb,int xfin,int yfin) {

  int index=0;
  int valeur;
  Image<int> q(p);
  int* PIX=p.PI();
  int L=p.PL();
  int W=p.PW();

  for (int j=0;j<L;j++)
    for (int i=0;i<W;i++) {
      if (erodil) PIX[index]=0; else PIX[index]=255;
      for (int a = xdeb;a <= xfin;a++)
        for (int b = ydeb;b <= yfin;b++) {
	  if (tab[a][b]) {
	    valeur = q.X(i-sens*(a-NMAX/2),j-sens*(b-NMAX/2));
	    if (tab[a][b]==2) valeur = 255-valeur;
	    if (erodil == 0)
	      PIX[index]=min(PIX[index],valeur);
	    else  PIX[index]=max(PIX[index],valeur); }
	}
      index++;
    }  
}

void MaxPropag(Image<int>& p,int conex){

int w = p.PW();
int h = p.PL();
Image<int> q(p);
Image<int> qtemp(p);
Image<int> pres(p);
int* Qpix=qtemp.PI();
int* Ppix=pres.PI();
int index,x,y,interior;
FIFO f(w*h);
FIFO g(w*h);
 
 pres = p;
 for (int j=0;j<h;j++)
   for (int i=0;i<w;i++){
     index = j*w + i;
     if (p.X(i,j)) {
       interior = p.X(i-1,j)&&p.X(i+1,j)&&p.X(i,j-1)&&p.X(i,j+1);
       if (conex == 8) interior = interior &&
			 p.X(i-1,j-1)&&p.X(i-1,j+1)&&p.X(i+1,j-1)&&p.X(i+1,j+1);
       if (!interior) {f.FIFO_add(i,j); Qpix[index]=0;Ppix[index]=0;}
     }
   }

 while (!f.FIFO_empty()) { 
   q = qtemp;
   while (!f.FIFO_empty()) {
     f.FIFO_get(x,y);
     if (qtemp.X(x-1,y)) {g.FIFO_add(x-1,y);Qpix[y*w+x-1]=0;Ppix[y*w+x-1]=0;}
     if (qtemp.X(x+1,y)) {g.FIFO_add(x+1,y);Qpix[y*w+x+1]=0;Ppix[y*w+x+1]=0;}
     if (qtemp.X(x,y-1)) {g.FIFO_add(x,y-1);Qpix[(y-1)*w+x]=0;Ppix[(y-1)*w+x]=0;}
     if (qtemp.X(x,y+1)) {g.FIFO_add(x,y+1);Qpix[(y+1)*w+x]=0;Ppix[(y+1)*w+x]=0;}
     if (conex == 8) {
       if (qtemp.X(x-1,y-1)) {g.FIFO_add(x-1,y-1);Qpix[(y-1)*w+x-1]=0;Ppix[(y-1)*w+x-1]=0;}
       if (qtemp.X(x+1,y-1)) {g.FIFO_add(x+1,y-1);Qpix[(y-1)*w+x+1]=0;Ppix[(y-1)*w+x+1]=0;}
       if (qtemp.X(x-1,y+1)) {g.FIFO_add(x-1,y+1);Qpix[(y+1)*w+x-1]=0;Ppix[(y+1)*w+x-1]=0;}
       if (qtemp.X(x+1,y+1)) {g.FIFO_add(x+1,y+1);Qpix[(y+1)*w+x+1]=0;Ppix[(y+1)*w+x+1]=0;}
     }
     interior = q.X(x-1,y)||q.X(x+1,y)||q.X(x,y-1)||q.X(x,y+1);
     if (conex != 4) interior = interior ||
		       q.X(x-1,y-1)||q.X(x+1,y-1)||q.X(x-1,y+1)||q.X(x+1,y+1);
     if (!interior) Ppix[y*w+x]=255;
   }
   f = g;
   g.FIFO_delete();
 }
 p = pres;
}

void Decale_Image(Image<int>& p,int vertical_offset) {
int w = p.PW();
int h = p.PL();
int* PIX=p.PI();
int index;

for (index=0;index<w*h;index++) 
   PIX[index] = min(255,PIX[index]+vertical_offset);
}

void Reconstruit(Image<int>& p,Image<int>& p_ref,int cx){
 
int w = p.PW();
int h = p.PL();
int* PIX=p.PI();
int index,newi,newj,newindex,initial,x,y;
FIFO f(w*h);

Max_regionaux(p,cx);

if (cx == 4) { 
 for (int j=0;j<h;j++)
   for (int i=0;i<w;i++){
     index = j * w + i;
     if ((PIX[index]) && !(p.X(i-1,j) && p.X(i,j-1) && p.X(i+1,j) && p.X(i,j+1))) f.FIFO_add(i,j);
     }
 while (!f.FIFO_empty()) {
      f.FIFO_get(x,y);
      for (int a=0;a<=1;a++)
        for (int b=-1;b<=1;b+=2) {
              newi = x-a*b; newj = y-(1-a)*b;
              if ((p.X(newi,newj) < p.X(x,y)) && (p_ref.X(newi,newj) != p.X(newi,newj)) && (newi>=0) && (newj>=0) && (newi<w) && (newj<h)) {
                   newindex = newj * w + newi;
                   PIX[newindex] = min(p.X(x,y),p_ref.X(newi,newj));
                   f.FIFO_add(newi,newj);}
               }
       }
 }
else {
for (int j=0;j<h;j++)
  for (int i=0;i<w;i++){
    index = j * w + i;
    initial=1;
    for (int a=-1;a<=1;a++)
      for (int b=-1;b<=1;b++)
        initial = initial && p.X(i-a,j-b);
    if (PIX[index] && !initial) f.FIFO_add(i,j);
    }
while (!f.FIFO_empty()) {
           f.FIFO_get(x,y);
           for (int a=-1;a<=1;a++)
            for (int b=-1;b<=1;b++) {
              if ((a!=0)||(b!=0)) {
                 newi = x-a; newj = y-b;
                 if ((p.X(newi,newj) < p.X(x,y)) && (p_ref.X(newi,newj) != p.X(newi,newj)) && (newi>=0) && (newj>=0) && (newi<w) && (newj<h)) {
                       newindex = newj * w + newi;
                       PIX[newindex] = min(p.X(x,y),p_ref.X(newi,newj));
                       f.FIFO_add(newi,newj);}
	          }
	     }
	}   
 }   
}

void Max_regionaux(Image<int>& p,int cx){
 
int w = p.PW();
int h = p.PL();
int* PIX=p.PI();
Image<int> q(p);
int index,newi,newj,indexnew,x,y,maxloc,newindex;
FIFO f(w*h);
 
if (cx == 4) { 
for (int j=0;j<h;j++)
  for (int i=0;i<w;i++){
    index = j * w + i;
    if ((PIX[index] < max(q.X(i-1,j),max(q.X(i,j-1),max(q.X(i+1,j),q.X(i,j+1))))) && (q.X(i,j))) {PIX[index]=0;f.FIFO_add(i,j);}
    while (!f.FIFO_empty()) {
                            f.FIFO_get(x,y);
                            for (int a=0;a<=1;a++)
                              for (int b=-1;b<=1;b+=2) {
                                 newi = x-a*b; newj = y-(1-a)*b;
                                 if ((q.X(newi,newj) <= q.X(x,y)) && (newi>=0) && (newj>=0) && (newi<w) && (newj<h)) {
                                  newindex = newj * w + newi;
                                  if (PIX[newindex]) {
                                 PIX[newindex] = 0;f.FIFO_add(newi,newj);}
                                 }
	                          }
	                     }
     }
   }
else {
  for (int j=0;j<h;j++)
  for (int i=0;i<w;i++){
    index = j * w + i;
    maxloc=1;
    for (int a=-1;a<=1;a++)
      for (int b=-1;b<=1;b++)
        maxloc = maxloc && (PIX[index] >= q.X(i-a,j-b));
    if ((!maxloc) && (q.X(i,j))) {PIX[index]=0;f.FIFO_add(i,j);}
    while (!f.FIFO_empty()) {
                            f.FIFO_get(x,y);
                            for (int a=-1;a<=1;a++)
                              for (int b=-1;b<=1;b++) {
                                if ((a!=0)||(b!=0)) {
                                   newi = x-a; newj = y-b;
                                   if ((q.X(newi,newj) <= q.X(x,y)) && (newi>=0) && (newj>=0) && (newi<w) && (newj<h)) {
                                   newindex = newj * w + newi;
                                   if (PIX[newindex]) {
                                            PIX[newindex] = 0;
                                            f.FIFO_add(newi,newj);}
                                  }
	                        }
	                      }
	                   }   
     }
   }   
 }    	   


int PutDistance(Image<int>& p,int i,int j,char *nom,int sens) {
  if (sens == 1) {
    if (strcmp(nom,"d4")==0) return min(p.X(i-1,j),p.X(i,j-1))+1;
    if (strcmp(nom,"d8")==0) return min(p.X(i-1,j-1),min(p.X(i,j-1),min(p.X(i+1,j-1),p.X(i-1,j))))+1;
    if (strcmp(nom,"ch3-4")==0) return min(p.X(i-1,j-1)+4,min(p.X(i,j-1)+3,min(p.X(i+1,j-1)+4,p.X(i-1,j)+3)));
    if (strcmp(nom,"ch5-7-11")==0)
      return min(min(min(min(min(p.X(i-1,j),p.X(i,j-1))+5,
                             min(p.X(i-1,j-1),p.X(i+1,j-1))+7),
                         min(p.X(i-2,j),p.X(i,j-2))+10),
                     min(p.X(i-2,j-1),min(p.X(i-1,j-2),min(p.X(i+1,j-2),p.X(i+2,j-1))))+11),
                 min(p.X(i-2,j-2),p.X(i+2,j-2))+14);
    if (strcmp(nom,"ch14-20-31-44")==0) 
      return min(min(min(min(min(min(min(min(min(p.X(i-1,j),p.X(i,j-1))+14,
					     min(p.X(i-1,j-1),p.X(i+1,j-1))+20),
					 min(p.X(i-2,j),p.X(i,j-2))+28),
				     min(p.X(i-2,j-1),min(p.X(i-1,j-2),min(p.X(i+1,j-2),p.X(i+2,j-1))))+31),
				 min(p.X(i-2,j-2),p.X(i+2,j-2))+40),
			     min(p.X(i,j-3),p.X(i-3,j))+42),
			 min(p.X(i-3,j-1),min(p.X(i-1,j-3),min(p.X(i+1,j-3),p.X(i+3,j-1))))+44),
		     min(p.X(i-3,j-2),min(p.X(i-2,j-3),min(p.X(i+2,j-3),p.X(i+3,j-2))))+51),
		 min(p.X(i-3,j-3),p.X(i+3,j-3))+60);
  } else {
    if (strcmp(nom,"d4")==0) return min(p.X(i,j),(min(p.X(i+1,j),p.X(i,j+1))+1));
    if (strcmp(nom,"d8")==0) return min(p.X(i,j),min(p.X(i+1,j+1),min(p.X(i,j+1),min(p.X(i-1,j+1),p.X(i+1,j))))+1);
    if (strcmp(nom,"ch3-4")==0) return min(p.X(i,j),min(p.X(i+1,j+1)+4,min(p.X(i,j+1)+3,min(p.X(i-1,j+1)+4,p.X(i+1,j)+3))));
    if (strcmp(nom,"ch5-7-11")==0)
      return min(p.X(i,j),min(min(min(min(min(p.X(i+1,j),p.X(i,j+1))+5,
                                          min(p.X(i+1,j+1),p.X(i-1,j+1))+7),
                                      min(p.X(i+2,j),p.X(i,j+2))+10),
                                  min(p.X(i+2,j+1),min(p.X(i+1,j+2),min(p.X(i-1,j+2),p.X(i-2,j+1))))+11),
                              min(p.X(i+2,j+2),p.X(i-2,j+2))+14));
    if (strcmp(nom,"ch14-20-31-44")==0) 
      return min(p.X(i,j),min(min(min(min(min(min(min(min(min(p.X(i+1,j),p.X(i,j+1))+14,
							  min(p.X(i+1,j+1),p.X(i-1,j+1))+20),
						      min(p.X(i+2,j),p.X(i,j+2))+28),
						  min(p.X(i+2,j+1),min(p.X(i+1,j+2),min(p.X(i-1,j+2),p.X(i-2,j+1))))+31),
					      min(p.X(i+2,j+2),p.X(i-2,j+2))+40),
					  min(p.X(i,j+3),p.X(i+3,j))+42),
				      min(p.X(i+3,j+1),min(p.X(i+1,j+3),min(p.X(i-1,j+3),p.X(i-3,j+1))))+44),
				  min(p.X(i+3,j+2),min(p.X(i+2,j+3),min(p.X(i-2,j+3),p.X(i-3,j+2))))+51),
			      min(p.X(i+3,j+3),p.X(i-3,j+3))+60));
  }
}
 
void FctDistance(Image<int>& p,char *nom) {
  int h=p.PL();
  int w=p.PW();
  int i,j;
  int index = 0;
  int *PIX = p.PI();
  int valmax = 0;
 
  for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if (*PIX > 0) *PIX = PutDistance(p,i,j,nom,1);
      PIX++;
        }             
  for (j=h-1;j>=0;j--)  
    for (i=w-1;i>=0;i--) { 
      PIX--;
      if (*PIX > 0) { 
        *PIX =  PutDistance(p,i,j,nom,-1); 
        if (*PIX > valmax) valmax = *PIX; 
         }
       }  
  // Affichage : réglage de la dynamique
  for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if (*PIX > 0) *PIX = (*PIX * 255)/valmax;
      PIX++;
       }
} 

void Maxlocaux(Image<int>& p,int conex) {
  int h=p.PL();
  int w=p.PW();
  int i,j,a,b,maxloc;
  Image<int> p_res(w,h);
  int *PIX = p_res.PI();
 
  if (conex == 4) {
    for (j=0;j<h;j++)
      for (i=0;i<w;i++)
	if (p.X(i,j) && (p.X(i,j)>=max(p.X(i-1,j),max(p.X(i,j-1),max(p.X(i+1,j),p.X(i,j+1)))))) PIX[j*w+i] = 255; else PIX[j*w+i] = 0;
  } else {
    for (j=0;j<h;j++)
      for (i=0;i<w;i++) {
	maxloc = p.X(i,j);
	for (a=-1;a<=1;a++)
	  for (b=-1;b<=1;b++)
	    maxloc = maxloc && (p.X(i,j) >= p.X(i-a,j-b));
	if (maxloc) PIX[j*w+i] = 255; else PIX[j*w+i] = 0;
      }
  }
  p = p_res;
} 

void Ero_ult(Image<int>& p,int conex) {
   int h=p.PL();
   int w=p.PW();
   int i,j;
   int *PIX = p.PI();
   int index = 0;
  char nom_dist[8];
   
   if (conex == 4) sprintf(nom_dist,"d4");    
        else sprintf(nom_dist,"d8");
   FctDistance(p,nom_dist);
   Max_regionaux(p,conex);
   for (j=0;j<h;j++)
     for (i=0;i<w;i++) {
       if (PIX[index] > 0) PIX[index] = 255;
       index++;
     }
 }


void Fct_distance_euclidienne(Image<int>& p_sdet) { 
  
   int h=p_sdet.PL();
   int w=p_sdet.PW();
   // R : coordonnées relatives horiz/vert du pixel le + proche
   Image<int> p_Rx(w,h);
   Image<int> p_Ry(w,h);
   Image<int> p_tempo(p_sdet);
   int i,j,index,indexbis,dir,a,b,sedt,valmax;
   int* PTEMP=p_tempo.PI();
   // f : distance au carré au pixel le + proche
   int* f=p_sdet.PI();
   int* RX=p_Rx.PI();
   int* RY=p_Ry.PI();


// Initialisation
   index = 0;
   for (j=0;j<h;j++) 
     for (i=0;i<w;i++) {
       RX[index]=0;
       RY[index]=0;
       if (!PTEMP[index])
	 f[index]=0;
       else
	 f[index]=1000000;
       index++;
     }

   // Balayage direct
   index = 0;
   indexbis = w;
   for (j=0;j<h;j++) {
     // 1er masque direct
     for (i=0;i<w;i++) {
       for (dir=0;dir<4;dir++) {
	 switch (dir) {
	 case 0 : {
	   a=-1;b=0;
	   sedt = p_sdet.X(i+a,j+b)+2*abs(p_Rx.X(i+a,j+b))+1;
	   break;}
	 case 1 : {
	   a=-1;b=-1;
	   sedt = p_sdet.X(i+a,j+b)+2*(abs(p_Rx.X(i+a,j+b))+abs(p_Ry.X(i+a,j+b))+1);
	   break;}
	 case 2 : {
	   a=0;b=-1;
	   sedt = p_sdet.X(i+a,j+b)+2*abs(p_Ry.X(i+a,j+b))+1;
	   break;}
	 case 3 : {
	   a=1;b=-1;
	   sedt = p_sdet.X(i+a,j+b)+2*(abs(p_Rx.X(i+a,j+b))+abs(p_Ry.X(i+a,j+b))+1);
	   break;}
	 }
	 if ((sedt<f[index])&&(i+a>=0)&&(j+b>=0)&&(i+a<w)&&(j+b<h)) {
	   f[index] = sedt;
	   RX[index] = p_Rx.X(i+a,j+b)+a;
	   RY[index] = p_Ry.X(i+a,j+b)+b;
	 }
       }
       index++;
     }
     // 2e masque direct, passé de droite à gauche
     for (i=w-1;i>=0;i--) {
       indexbis--;
       a=1;b=0;
       sedt = p_sdet.X(i+a,j+b)+2*abs(p_Rx.X(i+a,j+b))+1;
       if ((sedt<f[indexbis])&&(i+a>=0)&&(j+b>=0)&&(w-1-i+a<w)&&(j+b<h)) {
	 f[indexbis] = sedt;
	 RX[indexbis] = p_Rx.X(i+a,j+b)+a;
	 RY[indexbis] = p_Ry.X(i+a,j+b)+b;
       }
     }
     indexbis += 2*w;
   }
   // Balayage retrograde
   for (j=h-1;j>=0;j--) { 
     indexbis -= 2*w;
     // 1er masque rétrograde
     for (i=w-1;i>=0;i--) {
       index--;
       for (dir=0;dir<4;dir++) {
	 switch (dir) {
	 case 0 : {
	   a=1;b=0;
	   sedt = p_sdet.X(i+a,j+b)+2*abs(p_Rx.X(i+a,j+b))+1;
	   break;}
	 case 1 : {
	   a=1;b=1;
	   sedt = p_sdet.X(i+a,j+b)+2*(abs(p_Rx.X(i+a,j+b))+abs(p_Ry.X(i+a,j+b))+1);
	   break;}
	 case 2 : {
	   a=0;b=1;
	   sedt = p_sdet.X(i+a,j+b)+2*abs(p_Ry.X(i+a,j+b))+1;
	   break;}
	 case 3 : {
	   a=-1;b=1;
	   sedt = p_sdet.X(i+a,j+b)+2*(abs(p_Rx.X(i+a,j+b))+abs(p_Ry.X(i+a,j+b))+1);
	   break;}
	 }
	 if ((sedt<f[index])&&(i+a>=0)&&(j+b>=0)&&(i+a<w)&&(j+b<h)) {
	   f[index] = sedt;
	   RX[index] = p_Rx.X(i+a,j+b)+a;
	   RY[index] = p_Ry.X(i+a,j+b)+b;
	 }
       }
     }
     // 2e masque retrograde, passé de gauche à droite
     for (i=0;i<w;i++) {
       a=-1;b=0;
       sedt = p_sdet.X(i+a,j+b)+2*abs(p_Rx.X(i+a,j+b))+1;
       if ((sedt<f[indexbis])&&(i+a>=0)&&(j+b>=0)&&(i+a<w)&&(j+b<h)) {
	 f[indexbis] = sedt;
	 RX[indexbis] = p_Rx.X(i+a,j+b)+a;
	 RY[indexbis] = p_Ry.X(i+a,j+b)+b;
       }
       indexbis++;
     }
   }

   // Affichage des résultats : calcul de la racine carrée
   // et normalisation dans l'intervalle [0,255]
   
   index = 0;
   valmax = 0;
   for (j=0;j<h;j++)
     for (i=0;i<w;i++) {
       f[index] = (int)sqrt((double)f[index]);
       if (f[index]>valmax) valmax=f[index];
       index++;
     }
   index = 0;
   for (j=0;j<h;j++)
     for (i=0;i<w;i++) {
       f[index]=(f[index]*255)/valmax;
       index++;
     }
 }


void LPE(Image<int>& p,int conex) {

  int w = p.PW();
  int h = p.PL();
  Image<int> p_minima(p);
  Image<int> p_label(w,h);
  Image<int> p_done(w,h);
  Image<int> p_lpe(w,h);
  int* MIN=p_minima.PI();
  int* LAB=p_label.PI();
  int* DONE=p_done.PI();
  int* LPE=p_lpe.PI();
  int* PIX=p.PI();
  int index,newi,newj,x,y,minloc,newindex,indice,num;
  FIFO f(w*h);
  int i,j,a,b;
 
  printf("Connexité : %d\n",conex);
// (1) Calcul des minima régionaux
if (conex == 4) {
  index=0; 
  for (j=0;j<h;j++)
    for (i=0;i<w;i++){
      if (MIN[index] > min(p.X(i-1,j),min(p.X(i,j-1),min(p.X(i+1,j),p.X(i,j+1))))) {MIN[index]=506;f.FIFO_add(i,j);}
      while (!f.FIFO_empty()) {
	f.FIFO_get(x,y);
	for (a=0;a<=1;a++)
	  for (b=-1;b<=1;b+=2) {
	    newi = x-a*b; newj = y-(1-a)*b;
	    if ((p.X(newi,newj) >= p.X(x,y)) && (newi>=0) && (newj>=0) && (newi<w) && (newj<h)) {
	      newindex = newj * w + newi;
	      if (MIN[newindex]!=506) {
		MIN[newindex] = 506;f.FIFO_add(newi,newj);}
	    }
	  }
      }
      index++;
    }
} else {
  index=0;
  for (j=0;j<h;j++)
    for (i=0;i<w;i++){
      minloc=1;
      for (a=-1;a<=1;a++)
	for (b=-1;b<=1;b++)
	  minloc = minloc && (MIN[index] <= p.X(i-a,j-b));
      if (!minloc) {MIN[index]=506;f.FIFO_add(i,j);}
      while (!f.FIFO_empty()) {
	f.FIFO_get(x,y);
	for (a=-1;a<=1;a++)
	  for (b=-1;b<=1;b++) {
	    if ((a!=0)||(b!=0)) {
	      newi = x-a; newj = y-b;
	      if ((p.X(newi,newj) >= p.X(x,y)) && (newi>=0) && (newj>=0) && (newi<w) && (newj<h)) {
		newindex = newj * w + newi;
		if (MIN[newindex]!=506) {
		  MIN[newindex] = 506;
		  f.FIFO_add(newi,newj);}
	      }
	    }
	  }
      } 
      index++;
    }
}

// (2) Etiquettage des minima

 p_label = p_minima;
 p_done = p_minima;
 num = 0;
 if (conex == 4) {
  for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if (p_done.X(i,j) != 506) {
	indice = 1;
	LAB[j*w+i] = ++num;
	DONE[j*w+i] = 506;
	f.FIFO_add(i,j);
	while (!f.FIFO_empty()) {
	  f.FIFO_get(x,y);
	  for (a=0;a<=1;a++)
	    for (b=-1;b<=1;b+=2) {
	      newi = x-a*b; newj = y-(1-a)*b;
	      if ((p_done.X(newi,newj)!=506)&&(newi>=0)&&(newj>=0)&&(newi<w)&&(newj<h)) {
		  f.FIFO_add(newi,newj);
		  indice++;
		  index = newj*w+newi;
		  LAB[index]=num;
		  DONE[index]=506;
		}
	      }
	    }
	}
	printf("minimum n°%d : %d points\n",num,indice);
      }
    } else {
    for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if (p_done.X(i,j) != 506) {
	indice = 1;
	LAB[j*w+i] = ++num;
	DONE[j*w+i] = 506;
	f.FIFO_add(i,j);
	while (!f.FIFO_empty()) {
	  f.FIFO_get(x,y);
	  for (a=-1;a<=1;a++)
	    for (b=-1;b<=1;b++) {
	      if ((a!=0)||(b!=0)) {
		if ((p_done.X(x+a,y+b)!=506)&&(x+a>=0)&&(y+b>=0)&&(x+a<w)&&(y+b<h)) {
		  f.FIFO_add(x+a,y+b);
		  indice++;
		  index = (y+b)*w+x+a;
		  LAB[index]=num;
		  DONE[index]=506;
		}
	      }
	    }
	}
	printf("minimum n°%d : %d points\n",num,indice);
      }
    }
 }
   // Nombre de labels
  printf("%d labels\n",num);  
  
  // (3) Innondation à partir des minima

  num = 1;
 p_lpe = p_minima; 
if (conex == 4) {
  for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if (p_label.X(i,j) == num) {
	f.FIFO_add(i,j);
	DONE[j*w+i]=507;
	while (!f.FIFO_empty()) {
	  f.FIFO_get(x,y);
	  for (a=0;a<=1;a++)
	    for (b=-1;b<=1;b+=2) {
	      newi = x-a*b; newj = y-(1-a)*b;
	      if ((p_done.X(newi,newj)!=507)&&
		  (newi>=0)&&(newj>=0)&&(newi<w)&&(newj<h)&&
		  (p.X(newi,newj)>=p.X(x,y))) {
		indice = newj*w+newi;
		f.FIFO_add(newi,newj);
		DONE[indice] = 507;
		MIN[indice] = p_minima.X(x,y);
		if (LAB[indice] == 506)
		  LAB[indice] = num;
		LPE[indice]= MIN[indice];
	      }
	    }
	}
      }
      num++;
    }
 } else {
  for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if (p_label.X(i,j) == num) {
	f.FIFO_add(i,j);
	DONE[j*w+i]=507;
	while (!f.FIFO_empty()) {
	  f.FIFO_get(x,y);
	  for (a=-1;a<=1;a++)
	    for (b=-1;b<=1;b++) {
	      if ((a!=0)||(b!=0)) {
		if ((p_done.X(x+a,y+b)!=507)&&
		    (x+a>=0)&&(y+b>=0)&&(x+a<w)&&(y+b<h)&&
		    (p.X(x+a,y+b)>=p.X(x,y))) {
		  indice = (y+b)*w+x+a;
		  f.FIFO_add(x+a,y+b);
		  DONE[indice] = 507;
		  MIN[indice] = p_minima.X(x,y);
		  if (LAB[indice] == 506)
		    LAB[indice] = num;
		  LPE[indice]= MIN[indice];
		}
	      }
	    }
	}
	num++;
      }
    }
}
  // Affichage des frontières
  indice=0;
 //  for (j=0;j<h;j++)
//     for (i=0;i<w;i++) {
//       LPE[indice] = 0;
//       if (LAB[indice]<p_label.X(i-1,j))
// 	LPE[indice] = min(PIX[indice]-MIN[indice],PIX[indice]-p_minima.X(i-1,j));
//       if (LAB[indice]<p_label.X(i+1,j))
// 	LPE[indice] = max(LPE[indice],min(PIX[indice]-MIN[indice],PIX[indice]-p_minima.X(i+1,j)));
//       if (LAB[indice]<p_label.X(i,j-1))
// 	LPE[indice] = max(LPE[indice],min(PIX[indice]-MIN[indice],PIX[indice]-p_minima.X(i,j-1)));
//       if (LAB[indice]<p_label.X(i,j+1)) 
// 	LPE[indice] = max(LPE[indice],min(PIX[indice]-MIN[indice],PIX[indice]-p_minima.X(i,j+1)));
//       indice++;
//     }

if (conex == 4) {
 for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if ((LAB[indice]<p_label.X(i-1,j)) ||
	  (LAB[indice]<p_label.X(i+1,j)) ||
	  (LAB[indice]<p_label.X(i,j-1)) ||
	  (LAB[indice]<p_label.X(i,j+1)))
	LPE[indice] = 503;
      indice++;
    }
} else {
  for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if ((LAB[indice]<p_label.X(i-1,j)) ||
	  (LAB[indice]<p_label.X(i+1,j)) ||
	  (LAB[indice]<p_label.X(i,j-1)) ||
	  (LAB[indice]<p_label.X(i,j+1)) ||
	  (LAB[indice]<p_label.X(i-1,j-1)) ||
	  (LAB[indice]<p_label.X(i+1,j-1)) ||
	  (LAB[indice]<p_label.X(i-1,j+1)) ||
	  (LAB[indice]<p_label.X(i+1,j+1)))
	LPE[indice] = 503;
      indice++;
    }
}
  
  p = p_lpe;
}

void LPE_Markers(Image<int>& p,Image<int>& p_mark,int conex) {

  int w = p.PW();
  int h = p.PL();
  Image<int> p_label(w,h);
  Image<int> p_done(w,h);
  Image<int> p_lpe(w,h);
  int* LAB=p_label.PI();
  int* DONE=p_done.PI();
  int* LPE=p_lpe.PI();
  int* PIX=p.PI();
  int* MARK=p_mark.PI();
  int index,newi,newj,x,y,minloc,newindex,indice,num;
  FIFO f(w*h);
  int i,j,a,b;
 
  printf("Connexité : %d\n",conex);
// (1) Etiquetage des marqueurs

 p_label = p_mark;
 p_done = p_mark;
 num = 0;
 if (conex == 4) {
  for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if (p_done.X(i,j) != 0) {
	indice = 1;
	LAB[j*w+i] = ++num;
	DONE[j*w+i] = 0;
	f.FIFO_add(i,j);
	while (!f.FIFO_empty()) {
	  f.FIFO_get(x,y);
	  for (a=0;a<=1;a++)
	    for (b=-1;b<=1;b+=2) {
	      newi = x-a*b; newj = y-(1-a)*b;
	      if ((p_done.X(newi,newj)!=0)&&(newi>=0)&&(newj>=0)&&(newi<w)&&(newj<h)) {
		  f.FIFO_add(newi,newj);
		  indice++;
		  index = newj*w+newi;
		  LAB[index]=num;
		  DONE[index]=0;
		}
	      }
	    }
	}
	printf("marqueur n°%d : %d points\n",num,indice);
      }
    } else {
    for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if (p_done.X(i,j) != 0) {
	indice = 1;
	LAB[j*w+i] = ++num;
	DONE[j*w+i] = 0;
	f.FIFO_add(i,j);
	while (!f.FIFO_empty()) {
	  f.FIFO_get(x,y);
	  for (a=-1;a<=1;a++)
	    for (b=-1;b<=1;b++) {
	      if ((a!=0)||(b!=0)) {
		if ((p_done.X(x+a,y+b)!= 0)&&(x+a>=0)&&(y+b>=0)&&(x+a<w)&&(y+b<h)) {
		  f.FIFO_add(x+a,y+b);
		  indice++;
		  index = (y+b)*w+x+a;
		  LAB[index]=num;
		  DONE[index]=0;
		}
	      }
	    }
	}
	printf("marqueur n°%d : %d points\n",num,indice);
      }
    }
 }
   // Nombre de labels
  printf("%d labels\n",num);  
  
  // (3) Innondation à partir des marqueurs

  num = 1;
  p_lpe = p_mark; 
if (conex == 4) {
  for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if (p_label.X(i,j) == num) {
	f.FIFO_add(i,j);
	DONE[j*w+i]=507;
	while (!f.FIFO_empty()) {
	  f.FIFO_get(x,y);
	  for (a=0;a<=1;a++)
	    for (b=-1;b<=1;b+=2) {
	      newi = x-a*b; newj = y-(1-a)*b;
	      if ((p_done.X(newi,newj)!=507)&&
		  (newi>=0)&&(newj>=0)&&(newi<w)&&(newj<h)&&
		  (p.X(newi,newj)>=p.X(x,y))) {
		indice = newj*w+newi;
		f.FIFO_add(newi,newj);
		DONE[indice] = 507;
		if (LAB[indice] == 0)
		  LAB[indice] = num;
		LPE[indice]= MARK[indice];
	      }
	    }
	}
      }
      num++;
    }
 } else {
  for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if (p_label.X(i,j) == num) {
	f.FIFO_add(i,j);
	DONE[j*w+i]=507;
	while (!f.FIFO_empty()) {
	  f.FIFO_get(x,y);
	  for (a=-1;a<=1;a++)
	    for (b=-1;b<=1;b++) {
	      if ((a!=0)||(b!=0)) {
		if ((p_done.X(x+a,y+b)!=507)&&
		    (x+a>=0)&&(y+b>=0)&&(x+a<w)&&(y+b<h)&&
		    (p.X(x+a,y+b)>=p.X(x,y))) {
		  indice = (y+b)*w+x+a;
		  f.FIFO_add(x+a,y+b);
		  DONE[indice] = 507;
		  if (LAB[indice] == 0)
		    LAB[indice] = num;
		  LPE[indice]= MARK[indice];
		}
	      }
	    }
	}
	num++;
      }
    }
}
  // Affichage des frontières
  indice=0;
 //  for (j=0;j<h;j++)
//     for (i=0;i<w;i++) {
//       LPE[indice] = 0;
//       if (LAB[indice]<p_label.X(i-1,j))
// 	LPE[indice] = min(PIX[indice]-MIN[indice],PIX[indice]-p_minima.X(i-1,j));
//       if (LAB[indice]<p_label.X(i+1,j))
// 	LPE[indice] = max(LPE[indice],min(PIX[indice]-MIN[indice],PIX[indice]-p_minima.X(i+1,j)));
//       if (LAB[indice]<p_label.X(i,j-1))
// 	LPE[indice] = max(LPE[indice],min(PIX[indice]-MIN[indice],PIX[indice]-p_minima.X(i,j-1)));
//       if (LAB[indice]<p_label.X(i,j+1)) 
// 	LPE[indice] = max(LPE[indice],min(PIX[indice]-MIN[indice],PIX[indice]-p_minima.X(i,j+1)));
//       indice++;
//     }

if (conex == 4) {
 for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if ((LAB[indice]<p_label.X(i-1,j)) ||
	  (LAB[indice]<p_label.X(i+1,j)) ||
	  (LAB[indice]<p_label.X(i,j-1)) ||
	  (LAB[indice]<p_label.X(i,j+1)))
	LPE[indice] = 503;
      indice++;
    }
} else {
  for (j=0;j<h;j++)
    for (i=0;i<w;i++) {
      if ((LAB[indice]<p_label.X(i-1,j)) ||
	  (LAB[indice]<p_label.X(i+1,j)) ||
	  (LAB[indice]<p_label.X(i,j-1)) ||
	  (LAB[indice]<p_label.X(i,j+1)) ||
	  (LAB[indice]<p_label.X(i-1,j-1)) ||
	  (LAB[indice]<p_label.X(i+1,j-1)) ||
	  (LAB[indice]<p_label.X(i-1,j+1)) ||
	  (LAB[indice]<p_label.X(i+1,j+1)))
	LPE[indice] = 503;
      indice++;
    }
}
  
  p = p_lpe;
}

// Calcul de la dilatation rapide par un E.S. carré (méthode de Van Herk)
 
void Dilatation_VanHerk(Image<int>& p,int Rayon) {
  int width = p.PW();
  int height = p.PL();
  int i,j,index;
  Image<int> p_dilate1(width,height);
  Image<int> p_dilate2(width,height);
  int* TAB=p.PI();
  int* TAB_DILATE1=p_dilate1.PI();
  int* TAB_DILATE2=p_dilate2.PI();

  int Diametre = 2*Rayon + 1;

//Dilatation horizontale (méthode de Van Herk)
index = 0;// Phases 1 et 2 : dilatation bloc-récursive
for (j = 0;j < height;j++) {
  for (i = 0;i < width;i++) {
    if (i % Diametre == 0) TAB_DILATE1[index] = TAB[index];
      else TAB_DILATE1[index] = max(TAB[index],TAB_DILATE1[index-1]);
    index++;
  }
  for (i = width-1;i >= 0;i--) {
    index--;
    if ((i % Diametre == 0)||(i == width-1)) TAB_DILATE2[index] = TAB[index];
      else TAB_DILATE2[index] = max(TAB[index],TAB_DILATE2[index+1]);
  }
 index += width;
}
index = 0;// Phase 3 : Combinaison des max
for (j = 0;j < height;j++) {
  for (i = 0;i < Diametre/2;i++) {
    TAB[index] = max(TAB_DILATE1[index + Diametre/2],TAB_DILATE2[j*width]);
    index++;
    }
  for (i = Diametre/2;i < width-Diametre/2;i++) {
    TAB[index] = max(TAB_DILATE1[index + Diametre/2],TAB_DILATE2[index - Diametre/2]);
    index++;
  }
  for (i = width-Diametre/2;i < width;i++) {
    TAB[index] = max(TAB_DILATE1[(j+1)*width-1],TAB_DILATE2[index - Diametre/2]);
    index++;
  }
}
//Dilatation verticale (méthode de Van Herk)
// Phases 4 et 5 : dilatation bloc-récursive
for (i = 0;i < width;i++) {
  index = i;
  for (j = 0;j < height;j++) {
    if (j % Diametre == 0) TAB_DILATE1[index] = TAB[index];
      else TAB_DILATE1[index] = max(TAB[index],TAB_DILATE1[index-width]);
    index += width;
  }
  for (j = height-1;j >= 0;j--) {
    index -= width;
    if ((j % Diametre == 0)||(j == height-1)) TAB_DILATE2[index] = TAB[index];
      else TAB_DILATE2[index] = max(TAB[index],TAB_DILATE2[index+width]);
  }
}
// Phase 6 : Combinaison des max
for (i = 0;i < width;i++) {
  index = i;
  for (j = 0;j < Diametre/2;j++) {
    TAB[index] = max(TAB_DILATE1[index + width*(Diametre/2)],TAB_DILATE2[i]);
    index += width;
    }
  for (j = Diametre/2;j < height-Diametre/2;j++) {
    TAB[index] = max(TAB_DILATE1[index + width*(Diametre/2)],TAB_DILATE2[index - width*(Diametre/2)]);
    index += width;
  }
  for (j = height-Diametre/2;j < height;j++) {
    TAB[index] = max(TAB_DILATE1[width*(height-1)+i],TAB_DILATE2[index - width*(Diametre/2)]);
    index += width;
  }
}
}

// Calcul de l'érosion rapide par un E.S. carré (méthode de Van Herk)
 
void Erosion_VanHerk(Image<int>& p,int Rayon) {
  int width = p.PW();
  int height = p.PL();
  int i,j,index;
  Image<int> p_erode1(width,height);
  Image<int> p_erode2(width,height);
  int* TAB=p.PI();
  int* TAB_ERODE1=p_erode1.PI();
  int* TAB_ERODE2=p_erode2.PI();

  int Diametre = 2*Rayon + 1;

//Erosion horizontale (méthode de Van Herk)
index = 0;// Phases 1 et 2 : erosion bloc-récursive
for (j = 0;j < height;j++) {
  for (i = 0;i < width;i++) {
    if (i % Diametre == 0) TAB_ERODE1[index] = TAB[index];
      else TAB_ERODE1[index] = min(TAB[index],TAB_ERODE1[index-1]);
    index++;
  }
  for (i = width-1;i >= 0;i--) {
    index--;
    if ((i % Diametre == 0)||(i == width-1)) TAB_ERODE2[index] = TAB[index];
      else TAB_ERODE2[index] = min(TAB[index],TAB_ERODE2[index+1]);
  }
 index += width;
}
index = 0;// Phase 3 : Combinaison des min
for (j = 0;j < height;j++) {
  for (i = 0;i < Diametre/2;i++) {
    TAB[index] = min(TAB_ERODE1[index + Diametre/2],TAB_ERODE2[j*width]);
    index++;
    }
  for (i = Diametre/2;i < width-Diametre/2;i++) {
    TAB[index] = min(TAB_ERODE1[index + Diametre/2],TAB_ERODE2[index - Diametre/2]);
    index++;
  }
  for (i = width-Diametre/2;i < width;i++) {
    TAB[index] = min(TAB_ERODE1[(j+1)*width-1],TAB_ERODE2[index - Diametre/2]);
    index++;
  }
}
//Erosion verticale (méthode de Van Herk)
// Phases 4 et 5 : erosion bloc-récursive
for (i = 0;i < width;i++) {
  index = i;
  for (j = 0;j < height;j++) {
    if (j % Diametre == 0) TAB_ERODE1[index] = TAB[index];
      else TAB_ERODE1[index] = min(TAB[index],TAB_ERODE1[index-width]);
    index += width;
  }
  for (j = height-1;j >= 0;j--) {
    index -= width;
    if ((j % Diametre == 0)||(j == height-1)) TAB_ERODE2[index] = TAB[index];
      else TAB_ERODE2[index] = min(TAB[index],TAB_ERODE2[index+width]);
  }
}
// Phase 6 : Combinaison des min
for (i = 0;i < width;i++) {
  index = i;
  for (j = 0;j < Diametre/2;j++) {
    TAB[index] = min(TAB_ERODE1[index + width*(Diametre/2)],TAB_ERODE2[i]);
    index += width;
    }
  for (j = Diametre/2;j < height-Diametre/2;j++) {
    TAB[index] = min(TAB_ERODE1[index + width*(Diametre/2)],TAB_ERODE2[index - width*(Diametre/2)]);
    index += width;
  }
  for (j = height-Diametre/2;j < height;j++) {
    TAB[index] = min(TAB_ERODE1[width*(height-1)+i],TAB_ERODE2[index - width*(Diametre/2)]);
    index += width;
  }
}
}

