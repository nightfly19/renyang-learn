
set opt(trace) out.nam
set opt(nodes) 10
set opt(rtproto) DV
set opt(src.packet.size) 1000
set opt(src.packet.int) 0.005

set ns [new Simulator]
$ns rtproto $opt(rtproto)

set nf [open $opt(trace) w]
$ns namtrace-all $nf
$ns use-newtrace

proc finish {} {
	global ns nf opt
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

set tcpS0 [new Agent/TCP]
$ns attach-agent $n(0) $tcpS0
set tcpR0 [new Agent/TCPSink]
$ns attach-agent $n(2) $tcpR0

$ns connect $tcpS0 $tcpR0

set ftp [$tcpS0 attach-app FTP]

$ns at 0.5 "$ftp start"
$ns rtmodel-at 1.5 down $n(1)
$ns rtmodel-at 3.5 up $n(0) $n(1)
$ns rtmodel-at 5.5 up $n(1) $n(2)
$ns at 7.5 "$ftp stop"
$ns at 9.5 "finish"

$ns run
