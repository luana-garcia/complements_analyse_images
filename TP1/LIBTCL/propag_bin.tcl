proc com_base {nom} {
global p 
global q

if {$nom == "non"} {
    CMD non $p
} else {
    CMD $nom $p $q
}
}

proc lit_masque {} {
global v

  for {set j -4} {$j <= 4} {incr j} {
    for {set i -4} {$i <= 4} {incr i} {
       CMD setvaluemasque $i $j $v($i,$j)
           }
    }
}

proc Espejo_x {} {
global v

    for {set i -4} {$i <= 4} {incr i} {
	for {set j -4} {$j < 0} {incr j} {
	    set v($i,[expr $j * -1]) $v($i,$j)
	    update_but $i [expr $j * -1]
	}
}
}

proc Espejo_y {} {
global v

    for {set i -4} {$i < 0} {incr i} {
	for {set j -4} {$j <= 4} {incr j} {
	    set v([expr $i * -1],$j) $v($i,$j)
	    update_but [expr $i * -1] $j
    }
}
}

proc sym0x {} {
global v

    for {set i -4} {$i <= 4} {incr i} {
	for {set j -4} {$j < 0} {incr j} {
	    set tp $v($i,$j)
	    set v($i,$j) $v($i,[expr $j * -1])
	    update_but $i $j
	    set v($i,[expr $j * -1]) $tp 
	    update_but $i [expr $j * -1]
	}
    }
}

proc symOy {} {
global v

    for {set i -4} {$i < 0} {incr i} {
	for {set j -4} {$j <= 4} {incr j} {
	    set tp $v($i,$j)
	    set v($i,$j) $v([expr $i * -1],$j) 
	    update_but $i $j
	    set v([expr $i * -1],$j) $tp
	    update_but [expr $i * -1] $j
	}
    }
}

proc nettoie_elts {} {
global v

for {set i -4} {$i <= 4} {incr i} {
    for {set j -4} {$j <= 4} {incr j} {
	set v($i,$j) 0
	update_but $i $j
    }
}
}

proc setvalue {x y val} {
global v

    if {($v($x,$y)!=$val)} {set v($x,$y) $val} else {set v($x,$y) 0}
    update_but $x $y
}

proc update_but {x y} {
global v

    switch $v($x,$y) {
	0 {if {($x==0)||($y==0)} {set coul red} else {set coul orange}}
	1 {set coul white}
	2 {set coul black}
    }
    .b$x$y configure -bg $coul -activebackground $coul
}
    
proc app_morpho {action} {
global p

lit_masque 
CMD morpho $p $action
}

proc reconstruit {} {
global p
global q
global cx

    CMD reconstruit $p $q $cx
}

proc maxpropag {con} {
global p 
   CMD maxpropag $p $con 
}

proc seuil {} {
global p
global thr

CMD seuil $p $thr(th)
}

proc seuil_direct {niv} {
global p
global debut

    if {$debut == 0} { 
	CMD seuil $p $niv
    } else {
	set debut 0
    }
}

proc fctdist {nom} {
global p
    if {$nom == "euclide"} {
	CMD fde $p
    }  else {
	CMD fctdist $p $nom
    }
}

proc eroult {dist} {
global p

    CMD eroult $p $dist

}

proc maxloc {cx} {
global p

  CMD maxloc $p $cx

}

proc maxreg {cx} {
global p

  CMD maxreg $p $cx

}


proc lpe {} {
global p
global cx

  CMD lpe $p $cx

}

proc lpe_mark {} {
global p
global q
global cx

  CMD lpe_mark $p $q $cx

}
