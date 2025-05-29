#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tk.h>
#include "photo.hh"
#include "Image.hh"
#include "morfo.hh"
#include "tpmorpho1.hh"
#include "tpmorpho2.hh"

/* stockage des infos d'image */

Tk_PhotoImageBlock *blockPtr;

static int TAB[NMAX][NMAX];

template <class T> struct infoimage{
  Image<int> img;
  Tk_PhotoHandle handle;
};

/* Conversion en une image Inti avec allocation eventuelle */

Image<int> tkimage2Inti(Tk_PhotoImageBlock *tkim){
       int i,j;
       int *pixrim;
       unsigned char *pixtk;
       int tw=tkim->width;
       int th=tkim->height;
       Image<int> p(tw,th);

       pixrim=p.PI();
       for (j=0;j<tkim->height;j++) {
          pixtk=tkim->pixelPtr + tkim->pitch*j;
          for (i=0;i<tkim->width;i++) {
              int n = (int)((*(pixtk+tkim->offset[0]) + *(pixtk+tkim->offset[1]) + *(pixtk+tkim->offset[2]))/3);
              *pixrim = n;
              pixtk+=tkim->pixelSize;pixrim++;
          }
       }
       return p;
}

/* conversion en une image tk avec allocation eventuelle */

Tk_PhotoImageBlock *Inti2tkimage(Image<int>& pp,Tk_PhotoImageBlock *tkim) {
       unsigned char *tempotk;
       int *tempoplan;
       int i;
       if (tkim == NULL) {
              tkim = (Tk_PhotoImageBlock *)malloc(sizeof(Tk_PhotoImageBlock));
	      tkim->pixelPtr = NULL;
       }

       tkim->width=pp.PW();
       tkim->height=pp.PL();
       tkim->pixelSize=3;
       tkim->pitch=tkim->width*tkim->pixelSize;
       tkim->offset[0]=0;
       tkim->offset[1]=1;
       tkim->offset[2]=2;

       if (tkim->pixelPtr == NULL) {
	 tempotk= tkim->pixelPtr=(unsigned char *)malloc(tkim->pixelSize*tkim->width*tkim->height*sizeof(unsigned char));
       } else {
	 tempotk= tkim->pixelPtr=(unsigned char *)realloc(tkim->pixelPtr,tkim->pixelSize*tkim->width*tkim->height*sizeof(unsigned char));
       }
       tempoplan=pp.PI();
       for (i=0;i<tkim->width*tkim->height;i++) {
	 if (*tempoplan < 256) {
       	*tempotk = *(tempotk+tkim->offset[1]) = *(tempotk+tkim->offset[2]) = (unsigned char)(*tempoplan);
	 } else {
	   int c=(*tempoplan)-500;
	   *(tempotk+tkim->offset[0]) = 255*(c%2); 
	   *(tempotk+tkim->offset[1]) = 255*((c/2)%2);
	   *(tempotk+tkim->offset[2]) = 255*((c/4)%2);}
	tempoplan++;
	tempotk+=tkim->pixelSize;
       }
       return tkim;
}

void freetkim(Tk_PhotoImageBlock *tkim) {
       free(tkim->pixelPtr);
       free(tkim);
}



int Inti_cmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
       infoimage<int>* cdata;
       cdata  = (infoimage<int>*) clientData;
       
       float x;
       int a,b,c,d;
       int chg=1;
       int n;
       float T[3];
       char *message,*numb;
       
       if (argc > 2) a = atoi(argv[2]);
       if (argc > 3) b = atoi(argv[3]);  
       if (argc > 4) c = atoi(argv[4]); 
       if (argc > 5) d = atoi(argv[5]); 
       if (strncmp(argv[1],"change_bord",10) == 0) {
       ChangeBord(a);chg=0;}
       else
       if (strcmp(argv[1],"setvaluemasque") == 0) {
	 TAB[a+NMAX/2][b+NMAX/2]=c;chg=0;} 
       else
	 if (strcmp(argv[1],"save") == 0) {cdata[a].img.Imagetopgm(argv[3]);chg=0;}
	 else
	   if  (strcmp(argv[1],"defaire") == 0) {
	     cdata[a].img.DepileArriere();
	   }
	   else 
	     if  (strcmp(argv[1],"refaire") == 0) {
	       cdata[a].img.DepileAvant();
	     }
       else
	   {
	     if (strcmp(argv[1],"seuil") == 0) {
	       if (cdata[a].img.DerniereOperation() && (strcmp(cdata[a].img.DerniereOperation(),"seuil")==0))
	    {
	      cdata[a].img.DepileArriere();
	    }
	     }
	     cdata[a].img.Empile(argv[1]);
        if (strcmp(argv[1],"copy") == 0) {cdata[a].img = cdata[b].img;}  
        else
	if (strcmp(argv[1],"et") == 0) { cdata[a].img = cdata[a].img & cdata[b].img;}
	else
        if (strcmp(argv[1],"ou") == 0) { cdata[a].img = cdata[a].img | cdata[b].img;}
	else
	if (strcmp(argv[1],"xor") == 0) { cdata[a].img = cdata[a].img ^ cdata[b].img;}
	else
	if (strcmp(argv[1],"diff") == 0) { cdata[a].img = cdata[a].img.Image_diff(cdata[b].img);}
	else
	if (strcmp(argv[1],"non") == 0) {cdata[a].img = !cdata[a].img;}
	else
	if (strcmp(argv[1],"morpho") == 0) {Morphobase(cdata[a].img,argv[3],TAB);}
        else 
	if (strcmp(argv[1],"seuil") == 0) {cdata[a].img = cdata[a].img.Image_seuil(b);}
	else
        if (strcmp(argv[1],"decale_img") == 0) {Decale_Image(cdata[a].img,b);}
        else
	if (strcmp(argv[1],"reconstruit") == 0) {Reconstruit(cdata[a].img,cdata[b].img,c);}
	else 
	if (strcmp(argv[1],"eroult") == 0) {Ero_ult(cdata[a].img,b);}
	else
        if (strcmp(argv[1],"maxpropag") == 0) {MaxPropag(cdata[a].img,b);}
	else
        if (strcmp(argv[1],"maxloc") == 0) {Maxlocaux(cdata[a].img,b);}
        else
	if (strcmp(argv[1],"maxreg") == 0) {Max_regionaux(cdata[a].img,b);} 
        else
        if (strcmp(argv[1],"fctdist") == 0) {FctDistance(cdata[a].img,argv[3]);}  
	else
        if (strcmp(argv[1],"fde") == 0) {Fct_distance_euclidienne(cdata[a].img);}  
	else
        if (strcmp(argv[1],"lpe") == 0) {LPE(cdata[a].img,b);}
        else
        if (strcmp(argv[1],"lpe_mark") == 0) {LPE_Markers(cdata[a].img,cdata[b].img,c);}
        else
        if (strcmp(argv[1],"contrast") == 0) {Contraste(cdata[a].img,b,TAB);}
        else
        if (strcmp(argv[1],"fas") == 0) {FAS(cdata[a].img,b,c);}
        else
        if (strcmp(argv[1],"nivel") == 0) {Nivellement(cdata[a].img,cdata[b].img,c);}
        else
        if (strcmp(argv[1],"tp2_v1") == 0) {TP2_Version1(cdata[a].img,b,c);}
        else
        if (strcmp(argv[1],"tp2_v2") == 0) {TP2_Version2(cdata[a].img,cdata[b].img,c,d);}
        else
        if (strcmp(argv[1],"tp2_v3") == 0) {TP2_Version3(cdata[a].img,b,c,T);
                                            numb = (char*)malloc(15*sizeof(char));
                                            for (n = 0;n < 3;n++) {
	                                         sprintf(numb,"%f",T[n]);
	                                         Tcl_AppendElement(interp,numb);}
                                            }
        else {
                printf("Commande inconnue !\n");
		return TCL_ERROR;
	     }	   
         }
	    if (chg) {
       blockPtr=Inti2tkimage(cdata[a].img,blockPtr);
       Tk_PhotoPutBlock(cdata[a].handle, blockPtr, 0, 0, blockPtr->width, blockPtr->height);
       }
        return TCL_OK;       
}

int img_new(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
       infoimage<int> *cdata;
       char *error_msg;

       Tk_PhotoImageBlock blockPtr;
       int x = atoi(argv[2]);
       cdata = (infoimage<int>*) clientData;
     if (argc != 3) {
                sprintf(error_msg,"Usage: Imgnew <image> <numero>");
                Tcl_SetResult(interp,error_msg,TCL_VOLATILE);
		return TCL_ERROR;
	}
   cdata[x].handle=Tk_FindPhoto(interp,argv[1]);
   Tk_PhotoGetImage(cdata[x].handle, &blockPtr);
   cdata[x].img=tkimage2Inti(&blockPtr);
 return TCL_OK;
}


/* creation d'une image de type Inti */
/* Intinew <nomimage> <nomcommanderet> */

void init_retcc() {
blockPtr = NULL;
}

int Inti_new(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
       infoimage<int> *cdata;
       char *error_msg;

       if (argc != 3) {
         sprintf(error_msg,"Usage: Intinew <nomcommande> <nb_images>");
         Tcl_SetResult(interp,error_msg,TCL_VOLATILE);
	 return TCL_ERROR;
       }
       int nbimages = atoi(argv[2]);
       cdata = new infoimage<int>[nbimages];
       Tcl_CreateCommand(interp, "Imgnew", img_new, (ClientData)cdata,(Tcl_CmdDeleteProc *)NULL);
       Tcl_CreateCommand(interp, argv[1], Inti_cmd, (ClientData)cdata,(Tcl_CmdDeleteProc *)NULL);
       return TCL_OK;
}


