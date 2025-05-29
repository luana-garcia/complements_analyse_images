proc contraste {} {
global nbit
global p
  
   lit_masque
   CMD contrast $p $nbit

}

proc nivel {} {
global p
global q
global cx

  CMD nivel $p $q $cx

}

proc do_fas {prems} {
global nbit
global p

    CMD fas $p $prems $nbit

}

proc tp2_v1 {} {
    global p
    global nbit
    global thr
    
    CMD tp2_v1 $p $thr(th) $nbit
}

proc tp2_v2 {} {
    global p
    global q
    global nbit
    global thr
    
    CMD tp2_v2 $p $q $thr(th) $nbit
}

proc tp2_v3 {} {
    global p
    global nbit
    global thr
    
    set P [CMD tp2_v3 $p $thr(th) $nbit]
    set pmoins [format %.4g [lindex $P 0]]
    set pegal [format %.4g [lindex $P 1]]
    set pplus [format %.4g [lindex $P 2]]
    puts "$pmoins % de pixels ont une valeur inférieure à [expr $thr(th) - $nbit]"
    puts "$pegal % de pixels ont une valeur comprise entre [expr $thr(th) - $nbit] et [expr $thr(th) + $nbit]"
    puts "$pplus % de pixels ont une valeur supérieure à [expr $thr(th) + $nbit]"
}
