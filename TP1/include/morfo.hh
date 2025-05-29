#ifndef _MORFO_
#define _MORFO_

void Morphobase(Image<int>& p,char *nom,int tableau[NMAX][NMAX]);
void Appliq_morpho(Image<int>& p,int erodil,int sens,int tab[NMAX][NMAX],int xdeb,int ydeb,int xfin,int yfin);
void Decale_Image(Image<int>& p,int vertical_offset);

// Traitements locaux dans le 8 ou 4-voisinage
void Morpho_48(Image<int>& p,int erodil,int conex);
void Maxlocaux(Image<int>& p,int conex);

// Erosion et Dilatation rapides, E.S. carrés (méthode de Van Herk)
void Dilatation_VanHerk(Image<int>& p,int Rayon);
void Erosion_VanHerk(Image<int>& p,int Rayon);

// Reconstruction géodésique;
// Avec files d'attente 
void Reconstruit(Image<int>& p,Image<int>& p_ref,int cx);
void Max_regionaux(Image<int>& p,int cx);

// Squelette morphologique
// (maxima locaux de la fonction distance dconex)
void MaxPropag(Image<int>& p,int conex);

// Calcul de fonctions distance
int PutDistance(Image<int>& p,int i,int j,char *nom,int sens);
void FctDistance(Image<int>& p,char *nom); 
void Fct_distance_euclidienne(Image<int>& p_det);


//Calcul des érodés ultimes 
//(max régionaux de la fct distance dconex)
void Ero_ult(Image<int>& p,int conex);

// Watersheds
void LPE(Image<int>& p,int conex);
void LPE_Markers(Image<int>& p,Image<int>& p_mark,int conex);

#endif
