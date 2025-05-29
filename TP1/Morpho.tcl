#!/bin/sh
# \
exec ./intiwish "$0" "$@"
###############################################

#Mise a jour de la librairie TCL :
auto_mkindex ./LIBTCL *.tcl

lappend auto_path ./LIBTCL/

# Charge le package tkimg, si present
catch {package require Img}

set nbimages 4
set image ""
set maj 1
set debut 1

for {set i 0} {$i < $argc} {incr i} {
set str [lindex $argv $i]
     switch -- $str {
 	"-nb" {set nbimages [lindex $argv [expr $i +1]]}
 	"-i" {set image [lindex $argv [expr $i +1]]}
 	"-maj" {set maj [lindex $argv [expr $i +1]]}
     }
 }
 
Intinew CMD $nbimages 

label .planbin1
label .planbin2
set milieu [expr $nbimages/2]
for {set i 0} {$i < $nbimages} {incr i} {
image create photo toto($i) -file $image -palette 256/256/256
Imgnew toto($i) $i

canvas .cnv($i) -width [expr [image width toto($i)]*$maj] \
                -height [expr [image height toto($i)]*$maj]
.cnv($i) create image [expr [image width toto($i)]/2 + 1] \
	        [expr [image height toto($i)]/2 + 1] -image toto($i)
set x1($i) 0 
set y1($i) 0
set x2($i) [expr [image width toto(0)] - 1] 
set y2($i) [expr [image height toto(0)] - 1]
bind .cnv($i) <Button-1> "debutfenetre $i %x %y"
bind .cnv($i) <B1-Motion> "dessinefenetre $i %x %y"
bind .cnv($i) <ButtonRelease-1> "finfenetre $i %x %y"
bind .cnv($i) <Button-3> "debutsegment $i %x %y"
bind .cnv($i) <B3-Motion> "dessinesegment $i %x %y"
bind .cnv($i) <ButtonRelease-3> "finsegment $i %x %y"
label .op($i)
radiobutton .op1($i) -variable p -value $i -text "1"
radiobutton .op2($i) -variable q -value $i -text "2"
pack .op1($i) .op2($i) -in .op($i) -side left
    if {$i<$milieu} {
	pack .cnv($i) .op($i) -in .planbin1 -side top
    } else {
	pack .cnv($i) .op($i) -in .planbin2 -side top
    }
}

#update

set p 0
set q 1

set num 0
set nbit 1

label .act

label .menubar -relief sunken

menubutton .gene -text "Fichier" -menu .gene.menu
set m [menu .gene.menu]
$m add command -label "Défaire" -command "defaire"
$m add command -label "Refaire" -command "refaire"
$m add separator
$m add command -label "Original" -command "recup"
$m add command -label "Nouveau" -command "nouveau"
$m add command -label "Enregistrer" -command "save"
$m add command -label "Quitter" -command "exit"

menubutton .bin -text "Binaire" -menu .bin.menu
set m [menu .bin.menu]
$m add cascade -label "Transformées en distance" -menu $m.sub0
set m0 [menu $m.sub0 -tearoff 0]
$m0 add command -label "d4" -command "fctdist d4"
$m0 add command -label "d8" -command "fctdist d8"
$m0 add separator
$m0 add command -label "chamfrein 3-4" -command "fctdist ch3-4"
$m0 add command -label "chamfrein 5-7-11" -command "fctdist ch5-7-11"
$m0 add command -label "chamfrein 14-20-31-44" -command "fctdist ch14-20-31-44"
$m0 add separator
$m0 add command -label "distance euclidienne" -command "fctdist euclide"
$m add separator
$m add cascade -label "Squelette morphologique" -menu $m.sub1
set m1 [menu $m.sub1 -tearoff 0]
$m1 add command -label "d4" -command "maxpropag 4"
$m1 add command -label "d8" -command "maxpropag 8"
$m add separator
$m add cascade -label "Erodés ultimes" -menu $m.sub2
set m2 [menu $m.sub2 -tearoff 0]
$m2 add command -label "d4" -command "eroult 4"
$m2 add command -label "d8" -command "eroult 8"

menubutton .gray -text "Niveau de gris" -menu .gray.menu
set m [menu .gray.menu]
$m add cascade -label "Maxima locaux" -menu $m.sub1
set m1 [menu $m.sub1 -tearoff 0]
$m1 add command -label "d4" -command "maxloc 4"
$m1 add command -label "d8" -command "maxloc 8"
$m add separator
$m add cascade -label "Maxima régionaux" -menu $m.sub2
set m2 [menu $m.sub2 -tearoff 0]
$m2 add command -label "d4" -command "maxreg 4"
$m2 add command -label "d8" -command "maxreg 8"
$m add separator
$m add cascade -label "LPEs" -menu $m.sub3
set m3 [menu $m.sub3 -tearoff 0]
$m3 add command -label "LPE" -command "lpe"
$m3 add command -label "LPE avec Marqueurs" -command "lpe_mark"

menubutton .tp1 -text "TP n.1" -menu .tp1.menu
set m [menu .tp1.menu]
$m add command -label "Rehaussement de contraste" -command "contraste"
$m add separator
$m add command -label "Nivellement" -command "nivel"
$m add separator
$m add cascade -label "F.A.S" -menu $m.sub1
set m1 [menu $m.sub1 -tearoff 0]
$m1 add command -label "Ouverture d'abord" -command "do_fas 0"
$m1 add command -label "Fermeture d'abord" -command "do_fas 1"

menubutton .tp2 -text "TP n.2" -menu .tp2.menu
set m [menu .tp2.menu]
$m add command -label "Version n°1" -command "tp2_v1"
$m add command -label "Version n°2" -command "tp2_v2"
$m add command -label "Version n°3" -command "tp2_v3"

menubutton .opt -text "Options" -menu .opt.menu
set m [menu .opt.menu]
$m add cascade -label "Condition de bord" -menu $m.sub1
set m1 [menu $m.sub1 -tearoff 0]
$m1 add radio -label "bords nuls" -variable bord -value 0 -command "CMD change_bord 0"
$m1 add radio -label "bords géodésiques" -variable bord -value 1 -command "CMD change_bord 1"
$m1 add radio -label "bords wrappés" -variable bord -value 2 -command "CMD change_bord 2"
$m1 invoke 1
$m add cascade -label "Connexité par défaut" -menu $m.sub2
set m2 [menu $m.sub2 -tearoff 0]
$m2 add radio -label "4-connexité" -variable cx -value 4
$m2 add radio -label "8-connexité" -variable cx -value 8
$m2 invoke 1
$m add command -label "A propos..." -command "pub"

pack .gene .bin .gray .tp1 .tp2 .opt -in .menubar -side left

##################################################################
label .iterate -borderwidth 3 -bg blue -relief ridge -height 4 -width 28
label .tit -text "Paramètre 1"
place .tit -in .iterate -relx 0 -rely 0.5 -relheight 0.5 -relwidth 1
button .moinsun -command "decrem" -text "-"
place .moinsun -in .iterate -relx 0.2 -rely 0 -relheight 0.5 -relwidth 0.2
button .moins10 -command "dec10" -text "-10"
place .moins10 -in .iterate -relx 0 -rely 0 -relheight 0.5 -relwidth 0.2
button .resnbit -command "set nbit 1" -bg red -textvariable nbit
place .resnbit -in .iterate -relx 0.4 -rely 0 -relheight 0.5 -relwidth 0.2
button .plusun -command "increm" -text "+"
place .plusun -in .iterate -relx 0.6 -rely 0 -relheight 0.5 -relwidth 0.2
button .plus10 -command "inc10" -text "+10"
place .plus10 -in .iterate -relx 0.8 -rely 0 -relheight 0.5 -relwidth 0.2
###################################################################
##################################################################
label .nsip -height 5 -width 28 -borderwidth 3 -bg blue -relief ridge
set thr(step) 1
set thr(fin) 255
set thr(th) 128
label .thres
button .thrgo -text "Seuil" -command "seuil"
scale .thrsc -from 0 -to $thr(fin) \
    -variable thr(th) -orient horizontal \
    -resolution $thr(step) \
    -command "seuil_direct" 
pack .thrgo .thrsc -in .thres -side top -expand true -fill x
place .thres -in .nsip -relx 0 -rely 0 -relheight 1 -relwidth 1
###################################################################
label .zoom -borderwidth 3 -bg blue -relief ridge
button .zo -text "zoom x" -command "magnify"
set nx 2
scale .znb -from 2 -to 8 -variable nx -orient horizontal 
pack .zo .znb -in .zoom -side top -expand true -fill x
##################################################################
label .opvois -height 19 -width 28 -relief ridge -borderwidth 5 -bg blue
label .fr -text "Élément structurant" -height 2 -width 28 -bg green

label .elts -height 14 -width 28
for {set i -4} {$i <= 4} {incr i} {
 for {set j -4} {$j <= 4} {incr j} {
     if {($i == 0) || ($j == 0)} {
	 button .b$i$j -bg red -command "setvalue $i $j 1"
             #button .b$i$j -bg red -activebackground red -command "setvalue $i $j 1"
     } else {
	 button .b$i$j -bg orange -command "setvalue $i $j 1"
             #button .b$i$j -bg orange -activebackground orange -command "setvalue $i $j 1"
     } 
     bind .b$i$j <3> "setvalue $i $j 2"
     set v($i,$j) 0
  place .b$i$j -in .elts -relx [expr ($i+4)*0.1] \
                         -rely [expr ($j+4)*0.1] \
	                 -relheight 0.1 -relwidth 0.1
  }
 }
for {set i -1} {$i <= 1} {incr i} {
    for {set j -1} {$j <= 1} {incr j} {
	.b$i$j invoke
    }
}
button .esph -command "Espejo_x" -bg cyan -text "Miroir"
button .symh -command "sym0x" -bg lightblue -text "Symétrique"
place .esph -in .elts -relx 0 -rely 0.9 -relheight 0.1 -relwidth 0.45
place .symh -in .elts -relx 0.45 -rely 0.9 -relheight 0.1 -relwidth 0.45
button .espv -command "Espejo_y" -bg cyan -text "M"
button .symv -command "symOy" -bg lightblue -text "S"
place .espv -in .elts -relx 0.9 -rely 0 -relheight 0.45 -relwidth 0.1
place .symv -in .elts -relx 0.9 -rely 0.45 -relheight 0.45 -relwidth 0.1
button .zero -command "nettoie_elts" -bg red
place .zero -in .elts -relx 0.9 -rely 0.9 -relheight 0.1 -relwidth 0.1

label .morfo -height 3 -width 28
button .m1 -text "ERODE" -command "app_morpho erode"
button .m2 -text "DILATE" -command "app_morpho dilate"
pack .m1 .m2 -in .morfo -side left -expand true -fill x

pack .fr .elts .morfo -in .opvois -side top -expand true -fill x

##################################################################
label .operations -borderwidth 3 -bg blue -relief ridge
button .cp -text "copie" -command "copy"
label .fbase1 -height 3 -width 28
button .b1 -text "Non" -command "com_base non"
button .b2 -text "Inf" -command "com_base et"
button .b3 -text "Sup" -command "com_base ou"
pack .b1 .b2 .b3 -in .fbase1 -side left -expand true -fill x
label .fbase2 -height 3 -width 28
button .b4 -text "(x-y)" -command "com_base diff"
button .b5 -text "|x-y|" -command "com_base xor"
pack .b4 .b5 -in .fbase2 -side left -expand true -fill x
button .reco -text "reconstruction" -command "reconstruit"
button .dec -text "décale" -command "offset"
pack .cp .fbase1 .fbase2 .reco .dec -in .operations -side top -expand true -fill x

pack .menubar -side top -expand true -fill x

pack .zoom .iterate .nsip .opvois .operations -in .act -side top -expand true -fill x

pack .planbin1 .planbin2 .act -side left
