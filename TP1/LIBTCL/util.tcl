proc change_fmt {} {
global entree 
global f

regsub gif|pgm|ppm|tif|jpg|bmp $entree(result) $entree(format) entree(result)
}

proc Select_fichier {action} {
global entree
global f
 
set w .filebox
toplevel $w
wm title $w "Selection de fichiers"
wm iconname $w "filebox"
 
label $w.msg -wraplength 4i -justify left -text "Nom du fichier  ?"
pack $w.msg -side top
 
frame $w.buttons
pack $w.buttons -side bottom -fill x -pady 2m
button $w.buttons.cancel -text "Annuler" -command {set entree(ok) 0}
button $w.buttons.ok -text "OK" -command {set entree(ok) 1}
pack $w.buttons.cancel $w.buttons.ok -side left -expand 1
 
   set f [frame $w.$action]
if {$action == "enregistrer"} {
    set entree(result) "sans_titre.gif" 
} else {
    set entree(result) ""
}
    label $f.lab -text "$action : " -anchor e
    entry $f.ent -width 20 -textvar entree(result)
    bind $f.ent <Return> {set entree(ok) 1}
    button $f.but -text "Parcourir..." -command "fileDialog $w $f.ent $action"
    pack $f.lab -side left
    pack $f.ent -side left -expand yes -fill x
    pack $f.but -side left
    pack $f -fill x -padx 1c -pady 3
 
    checkbutton $w.strict -text "Look Motif" \
        -variable tk_strictMotif -onvalue 1 -offvalue 0
    pack $w.strict -anchor c
 if {$action == "enregistrer"} {
    set g [label $w.form_choice]
   label $g.texte -text "Type de fichier"
   radiobutton $g.gif -variable entree(format) -value "gif" -text "gif" -command "change_fmt"
   radiobutton $g.pgm -variable entree(format) -value "pgm" -text "pgm" -command "change_fmt"
   radiobutton $g.ppm -variable entree(format) -value "ppm" -text "ppm" -command "change_fmt"
   radiobutton $g.tiff -variable entree(format) -value "tif" -text "tiff" -command "change_fmt"
   radiobutton $g.jpg -variable entree(format) -value "jpg" -text "jpg" -command "change_fmt"
   radiobutton $g.bmp -variable entree(format) -value "bmp" -text "bmp" -command "change_fmt"
set entree(format) "gif"
pack $g.texte -in $w.form_choice -side top
pack $g.gif $g.pgm $g.ppm $g.tiff $g.jpg $g.bmp -in $w.form_choice -side left  
pack $g -side top
}
focus $f.ent
grab $w
tkwait variable entree(ok)
grab release $w
destroy $w
}     

proc fileDialog {w ent operation} {
global entree
    #   Type names              Extension(s)    Mac File Type(s)
    #
    #---------------------------------------------------------
 if {$operation == "enregistrer"} {
     set types {
         {"Image"   {.gif .pgm .ppm .tif .jpg .bmp}     }
         {"Tout fichier"           *}
     }
 set entree(result) [tk_getSaveFile -filetypes $types -parent $w \
 -initialfile "sans_titre" -defaultextension .$entree(format) -title "save"]
 } else {
     set types {
         {"Images gif"          {.gif}          }
         {"Images pgm"          {.pgm}          }
         {"Images ppm"          {.ppm}          }
         {"Images jpeg"         {.jpg .jpeg}            }
         {"Images tiff"         {.tif .tiff}            }
         {"Images bmp"          {.bmp}          }
         {"Other files"         *} 
 }
  set entree(result) [tk_getOpenFile -filetypes $types -parent $w \
                          -title $operation]
    }
}         

proc nouveau {} {
global nbimages
global image
global entree
global maj
global x2
global y2
 
    Select_fichier ouvrir
    if {$entree(ok)} {
	for {set i 0} {$i < $nbimages} {incr i} {
	    image create photo toto($i) -file $entree(result) -palette 256/256/256
	    Imgnew toto($i) $i
	    .cnv($i) configure \
		-width [expr [image width toto($i)]*$maj] \
		-height [expr [image height toto($i)]*$maj]
	    .cnv($i) create image [expr [image width toto($i)]/2 + 1] \
	        [expr [image height toto($i)]/2 + 1] -image toto($i)
	    set x2($i) [expr [image width toto($i)] - 1] 
	    set y2($i) [expr [image height toto($i)] - 1]
	}
	update
	set image $entree(result)
    }
}      

proc save {} {
global entree
global p

Select_fichier enregistrer
    if {$entree(ok)} {
     set titi toto($p)
  switch $entree(format) {
    "ppm" {$titi write $entree(result) -format PPM/PGM}
    "pgm" {CMD save $p $entree(result)}
    "gif" {$titi write $entree(result) -format GIF}
    "tif" {$titi write $entree(result) -format TIFF}
    "bmp" {$titi write $entree(result) -format BMP}
    "jpg" {$titi write $entree(result) -format JPEG}
}
}
}

proc recup {} {
global p
global image

image create photo toto($p) -file $image -palette 256/256/256
Imgnew toto($p) $p
update
}

proc defaire {} {
global p

CMD defaire $p
update
}

proc refaire {} {
global p

CMD refaire $p
update
}

proc decrem {} {
global nbit
    set nbit [expr $nbit - 1]
    if {$nbit < -100} {
   set nbit -100
    }
}

proc increm {} {
global nbit
    incr nbit
    if {$nbit > 100} {
  set nbit 100
    }
}

proc dec10 {} {
global nbit
    decrem 
    while {[expr $nbit % 10] != 0} {set  nbit [expr $nbit - 1]}
    if {$nbit < -100} {set nbit -100}
}

proc inc10 {} {
global nbit
     increm
    while {[expr $nbit % 10] != 0} {incr nbit}
  if {$nbit > 100} {set nbit 100}
}

proc message {text} {

toplevel .msg
wm title .msg "Message"

label .msg.corps -wraplength 4i -justify left -text $text
pack .msg.corps -side top
 
button .msg.button -text "OK" -command {set message_lu 1}
pack .msg.button -side left -expand 1
 
grab .msg
tkwait variable message_lu
grab release .msg
destroy .msg

}

proc copy {} {
global p
global q

CMD copy $p $q
}

proc offset {} {
global p
global nbit

CMD decale_img $p $nbit
}


proc magnify {} {

global x1 
global y1
global x2 
global y2
global nx
global p
 
set petit toto($p)
set gros bigtoto($p)
image create photo $gros -height [expr $nx*($y2($p) - $y1($p))] \
                         -width [expr $nx*($x2($p) - $x1($p))] \
                         -palette 256/256/256                                       
$gros copy $petit -from $x1($p) $y1($p) $x2($p) $y2($p) -zoom $nx
set win .zo_$gros
if {[winfo exists $win]} {
    set titre "Zoom ($x1($p),$y1($p)) -> ($x2($p),$y2($p))"
    wm title $win $titre
} else {
toplevel $win
set titre "Zoom ($x1($p),$y1($p)) -> ($x2($p),$y2($p))"
wm title $win $titre
 
label $win.img -image $gros
pack $win.img -side top
 
button $win.button -text "Fermer" -command "image delete $gros; \
                                            destroy $win"
pack $win.button -side left -expand 1
}                                      
}

proc debutfenetre {num x y} {

global x1 
global y1
global box_anchor
global box_last

set x1($num) $x
set y1($num) $y
catch {.cnv($num) delete $box_last($num)}
set box_anchor($num) [list $x $y]
catch {unset box_last($num)}
}

proc dessinefenetre {num x y} {
global box_anchor
global box_last

catch {.cnv($num) delete $box_last($num)}
set box_last($num) [eval {.cnv($num) create rect} $box_anchor($num) \
			{$x $y -tag box -outline red}]
}

proc finfenetre {num x y} {

global x1 
global y1
global x2 
global y2

set x2($num) $x
set y2($num) $y

set xmax [expr [image width toto($num)] - 1]
set ymax [expr [image height toto($num)] - 1]
if {$x1($num) > $x2($num)} {set tp $x1($num)
                set x1($num) $x2($num)
                set x2($num) $tp}
if {$y1($num) > $y2($num)} {set tp $y1($num)
                set y1($num) $y2($num)
                set y2($num) $tp}
if {$x2($num) > $xmax} {set x2($num) $xmax}
if {$y2($num) > $ymax} {set y2($num) $ymax}
if {$x1($num) < 0} {set x1($num) 0} 
if {$y1($num) < 0} {set y1($num) 0} 
if {($x1($num) == $x2($num)) || ($y1($num) == $y2($num))} {
set x1($num) 0
set y1($num) 0
set x2($num) $xmax
set y2($num) $ymax
}
}

proc pub {} {

toplevel .pub
wm title .pub "Info Inti0.1"

image create photo logo -file ./.intilogo.gif -palette 256/256/256

label .pub.img -image logo
pack .pub.img -side top
 
button .pub.button -text "Fermer" -command {set pub_vue 1}
pack .pub.button -side left -expand 1
 
grab .pub
tkwait variable pub_vue
grab release .pub
destroy .pub

}

proc debutsegment {num x y} {

global x1_profil 
global y1_profil
global segment_anchor
global segment_last

if {[winfo exists .plotprof_$num]} {
    destroy .plotprof_$num
}
set x1_profil($num) $x 
set y1_profil($num) $y
set xmax [expr [image width toto($num)] - 1]
set ymax [expr [image height toto($num)] - 1]
if {$x1_profil($num) > $xmax} {set x1_profil($num) $xmax}
if {$y1_profil($num) > $ymax} {set y1_profil($num) $ymax}
if {$x1_profil($num) < 0} {set x1_profil($num) 0}
if {$y1_profil($num) < 0} {set y1_profil($num) 0}
catch {.cnv($num) delete $segment_last($num)}
set segment_anchor($num) [list $x $y]
catch {unset segment_last($num)}
}

proc dessinesegment {num x y} {
global segment_anchor
global segment_last
global segment_tag

catch {.cnv($num) delete $segment_last($num)}
set segment_last($num) [eval {.cnv($num) create line} $segment_anchor($num) {$x $y -tag segment_tag($num) -fill yellow -arrow last}]
}

proc finsegment {num x y} {

global x1_profil 
global y1_profil
global x2_profil 
global y2_profil

set x2_profil($num) $x
set y2_profil($num) $y
set xmax [expr [image width toto($num)] - 1]
set ymax [expr [image height toto($num)] - 1]
if {$x2_profil($num) > [expr $xmax-1]} {set x2_profil($num) [expr $xmax-1]}
if {$y2_profil($num) > [expr $ymax-1]} {set y2_profil($num) [expr $ymax-1]}
if {$x2_profil($num) < 1} {set x2_profil($num) 1}
if {$y2_profil($num) < 1} {set y2_profil($num) 1}
#puts "($x2_profil($num),$y2_profil($num))"
if {($x2_profil($num) != $x1_profil($num)) || \
	($y2_profil($num) != $y1_profil($num))} {
    draw_profile $num
}
}

proc draw_profile {num} {

global profil_color
global x1_profil 
global y1_profil
global x2_profil 
global y2_profil

set profil_color($num) 0
set winpr [toplevel .plotprof_$num]
wm title $winpr "Profil Image $num ($x1_profil($num),$y1_profil($num)) -> ($x2_profil($num),$y2_profil($num))"
# Calcul de la largeur de fenetre
if {[expr abs($x2_profil($num)-$x1_profil($num))]>[expr abs($y2_profil($num)-$y1_profil($num))]} {
    set largeur [expr abs($x2_profil($num)-$x1_profil($num))]
} else {
    set largeur [expr abs($y2_profil($num)-$y1_profil($num))]
}
puts "Profil : $largeur points"
canvas $winpr.cadre -bg lightgrey
canvas $winpr.body -width $largeur -height 256
pack $winpr.body -in $winpr.cadre -padx 15 -pady 15
pack $winpr.cadre -side top
$winpr.body create line 0 256 $largeur 256 -fill lightgrey -dash 3
$winpr.body create line 0 206 $largeur 206 -fill lightgrey -dash 3
$winpr.body create line 0 156 $largeur 156 -fill lightgrey -dash 3
$winpr.body create line 0 106 $largeur 106 -fill lightgrey -dash 3
$winpr.body create line 0 55 $largeur 56 -fill lightgrey -dash 3
$winpr.body create line 0 6 $largeur 6 -fill lightgrey -dash 3
$winpr.cadre create line 14 286 14 0 -fill black -arrow last
$winpr.cadre create line 0 273 [expr $largeur + 30] 273  \
    -fill black -arrow last
button $winpr.but1 -text "Fermer" -command "destroy $winpr"
button $winpr.but2 -text "Update" -command "update_profile $num"
pack $winpr.but1 $winpr.but2 -side top

update_profile $num
}


proc update_profile {num} {

global profil_color
global x1_profil 
global y1_profil
global x2_profil 
global y2_profil

set winpr .plotprof_$num
switch $profil_color($num) {
    0 {set plot_color "black"}
    1 {set plot_color "magenta"}
    2 {set plot_color "green"}
    3 {set plot_color "orange"}
    4 {set plot_color "red"}
    5 {set plot_color "blue"}
    6 {set plot_color "purple"}
    7 {set plot_color "cyan"}
    8 {set plot_color "brown"}
    9 {set plot_color "darkgrey"}
}
incr profil_color($num)
if {$profil_color($num)>9} {set profil_color($num) 0}
# calcul du quadrant
if {$x1_profil($num) < $x2_profil($num)} {set sens_x 1} else {set sens_x -1}
if {$y1_profil($num) < $y2_profil($num)} {set sens_y 1} else {set sens_y -1}
# Calcul de l'octant
set absy [expr abs($y2_profil($num)-$y1_profil($num))]
set absx [expr abs($x2_profil($num)-$x1_profil($num))]
# Initialisations diverses...
set index 1
set x_profil $x1_profil($num)  
set y_profil $y1_profil($num)
set last_val [lindex [toto($num) get $x1_profil($num) $y1_profil($num)] 0]
set indic_x [expr $sens_x*($x2_profil($num)-$x_profil)]
set indic_y [expr $sens_y*($y2_profil($num)-$y_profil)]
if {$absx > $absy} {
    set pente [expr double($y2_profil($num)-$y1_profil($num))/ \
		   double($x2_profil($num)-$x1_profil($num))]
    #puts $pente
    while {($indic_x>=0)&&($indic_y>=0)} {
	set denom [expr double($x_profil+$sens_x-$x1_profil($num))]
	set match_horiz [expr abs((double($y_profil-$y1_profil($num))/$denom)-$pente)]
	set match_both [expr abs((double($y_profil+$sens_y-$y1_profil($num))/$denom)-$pente)]
	incr x_profil $sens_x
	if {$match_both < $match_horiz} {
	    incr y_profil $sens_y
	}
	set val [lindex [toto($num) get $x_profil $y_profil] 0]
	set indic_x [expr $sens_x*($x2_profil($num)-$x_profil)]
	set indic_y [expr $sens_y*($y2_profil($num)-$y_profil)]
	$winpr.body create line $index [expr 256 -$last_val] \
	    [expr $index + 1] [expr 256 - $val] -fill $plot_color
	incr index
	set last_val $val
	#puts "($x_profil,$y_profil):$val"
    }
} else {
    set antipente [expr double($x2_profil($num)-$x1_profil($num))/ \
		       double($y2_profil($num)-$y1_profil($num))]
    #puts $antipente
    while {($indic_x>=0)&&($indic_y>=0)} {
	set denom [expr double($y_profil+$sens_y-$y1_profil($num))]
	set match_vert [expr abs((double($x_profil-$x1_profil($num))/$denom)-$antipente)]
	set match_both [expr abs((double($x_profil+$sens_x-$x1_profil($num))/$denom)-$antipente)]
	incr y_profil $sens_y
	if {$match_both < $match_vert} {
	    incr x_profil $sens_x
	}
	set val [lindex [toto($num) get $x_profil $y_profil] 0]
	set indic_x [expr $sens_x*($x2_profil($num)-$x_profil)]
	set indic_y [expr $sens_y*($y2_profil($num)-$y_profil)]
	$winpr.body create line $index [expr 256 -$last_val] \
	    [expr $index + 1] [expr 256 - $val] -fill $plot_color
	incr index
	set last_val $val
	#puts "($x_profil,$y_profil):$val"
    }
}
}
