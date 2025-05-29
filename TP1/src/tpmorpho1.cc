
#include <string.h>
#include <stdio.h>
#include "Image.hh"
#include "morfo.hh"
#include "tpmorpho1.hh"

static int abs(int a) {if (a<0) return -a; else return a;}
static int max(int a,int b) {if (a>b) return a; else return b;}
static int min(int a,int b) {if (a<b) return a; else return b;}

// Quelques facilités : (1) déclarations d'images :
// Image<int> p(w,h); pour une image de largeur w et de hauteur h
// Image<int> p(q); image p initialisée à q
// (2) Données de bases des images :
// img.PL() : de type int, fournit la hauteur de l'image img
// img.PW() : de type int, fournit la largeur de l'image img
// img.PI() : de type int*, permet d'accéder en écriture aux données
// de l'image img, sous la forme d'un vecteur monodimensionnel,
// ex : mettre à 12 le pixel (105,22) de l'image img :
// img.PI()[img.PW()*22 + 105] = 12;
// Accès aux données en lecture seulement :
// on utilise la méthode X : img.X(x,y) renvoie la valeur du
// pixel (x,y) de l'image img (et gère automatiquement les
// problèmes de bord !)
// (3) Quelques opérations déja écrites;
// p = q; affectation, pixel à pixel, de l'image q à p
// p = q & r; Inf de deux images
// p = q | r; Sup de deux images 
// etc... (voir fichier Image.hh)

void Exemples_convolutions(Image<int>& p,char *nom) {

  int index,valeur;
  Image<int> q(p);
  int* PIX=p.PI();
  int H=p.PL();
  int W=p.PW();

  if (strcmp(nom,"moyenne")==0) {
    index = 0;
    for (int j=0;j<H;j++)
      for (int i=0;i<W;i++) {
	PIX[index]=(q.X(i,j)+q.X(i-1,j)+q.X(i,j-1)+q.X(i+1,j)+q.X(i,j+1))/5;
	index++;
      }
  } else if (strcmp(nom,"laplacien")==0) {
    index = 0;
    for (int j=0;j<H;j++)
      for (int i=0;i<W;i++) {
	valeur = 128+q.X(i-1,j)+q.X(i,j-1)+q.X(i+1,j)+q.X(i,j+1)-4*q.X(i,j);
	PIX[index]=min(255,max(0,valeur));
	index++;
      }
  } 
}

void Contraste(Image<int>& p,int type,int tableau[NMAX][NMAX]) {
  char operation[8];

  // test...
  printf("Rehaussement de contraste, type = %d\n",type);
  if (type == 0) sprintf(operation,"erode");
            else sprintf(operation,"dilate");
  Morphobase(p,operation,tableau);//Erosion ou dilatation (Elt. str. défini sur l'interface)
  
}


void Nivellement(Image<int>& p,Image<int>& p_ref,int cx) {

  // test...
  printf("Nivellement, connexité = %d\n",cx);
}

void FAS(Image<int>& p,int premier,int rayon) {

  // test...
  if (premier == 0) { 
    printf("Filtre alterné séquentiel, ouverture d'abord, taille = %d\n",rayon);
    Erosion_VanHerk(p,rayon);//Erosion rapide (E.S. carré), méthode de Van Herk
   } else {
    printf("Filtre alterné séquentiel, fermeture d'abord, taille = %d\n",rayon);
    Dilatation_VanHerk(p,rayon);//Dilatation rapide (E.S. carré), méthode de Van Herk
   }
}
