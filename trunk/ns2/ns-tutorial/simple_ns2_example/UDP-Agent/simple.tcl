
set opt(trace) out.nam
set opt(nodes) 10
set opt(rtproto) DV
set opt(src.packet.size) 800
set opt(src.packet.int) 0.5
set opt(udp.MMS) 100
set opt(udp.dest.addr) 1
set opt(udp.dest.port) 80
set opt(udp.ttl) 6

set ns [new Simulator]
$ns rtproto $opt(rtproto)

$ns color 1 Blue

set nf [open $opt(trace) w]
$ns namtrace-all $nf
$ns use-newtrace

proc finish {} {
	global ns opt nf
	$ns flush-trace
	close $nf
	exec nam $opt(trace) &
	exit 0
}

for {set i 0} {$i<$opt(nodes)} {incr i} {
	set n($i) [$ns node]
}

for {set i 0} {$i<$opt(nodes)} {incr i} {
	$ns duplex-link $n($i) $n([expr ($i+1)%$opt(nodes)]) 1Mb 10ms DropTail
}

set udp0 [new Agent/UDP]
$udp0 set packetSize_ $opt(udp.MMS)
$udp0 set dst_addr_ $opt(udp.dest.addr)
$udp0 set dst_port_ $opt(udp.dest.port)
$udp0 set ttl_ $opt(udp.ttl)
$udp0 set class_ 1

$ns attach-agent $n(0) $udp0

set cbr0 [new Application/Traffic/CBR]
$cbr0 set packetSize_ $opt(src.packet.size)
$cbr0 set interval_ $opt(src.packet.int)
$cbr0 attach-agent $udp0

set null0 [new Agent/Null]
$ns attach-agent $n(2) $null0

$ns connect $udp0 $null0

$ns at 0.5 "$cbr0 start"
$ns rtmodel-at 1.5 down $n(1)
$ns rtmodel-at 3.5 up $n(0) $n(1)
$ns rtmodel-at 5.5 up $n(1) $n(2)
$ns at 9.5 "finish"

$ns run
