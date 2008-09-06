
set ns [new Simulator]

$ns use-newtrace

set namtrace [open wireless_basic.nam w]
$ns namtrace-all-wireless $namtrace 670 670

set tracefd [open wireless_basic.tr w]
$ns trace-all $tracefd

proc finish {} {
	global ns namtrace tracefd
	$ns flush-trace
	close $namtrace
	close $tracefd
	exec nam wireless_basic.nam &
	exit 0
}

set topo [new Topography]
$topo load_flatgrid 670 670

create-god 2

set chan_ [new Channel/WirelessChannel]

$ns node-config -adhocRouting AODV \
		-llType LL \
		-macType Mac/Simple \
		-ifqType Queue/DropTail/PriQueue \
		-ifqLen 50 \
		-antType Antenna/OmniAntenna \
		-propType Propagation/TwoRayGround \
		-phyType Phy/WirelessPhy \
		-topoInstance $topo \
		-agentTrace ON \
		-routerTrace ON \
		-macTrace ON \
		-movementTrace ON \
		-channel $chan_

for {set i 0} {$i<2} {incr i} {
	set node($i) [$ns node]
	$node($i) random-motion 0
	$ns initial_node_pos $node($i) 20
}

$node(0) set X_ 15.0
$node(0) set Y_ 15.0
$node(0) set Z_ 0.0

$node(0) set X_ 150.0
$node(0) set Y_ 150.0
$node(0) set Z_ 0.0

$ns at 0.0 "$node(0) setdest 50.0 50.0 5.0"
$ns at 0.0 "$node(1) setdest 60.0 40.0 10.0"

$ns at 3.0 "$node(1) setdest 240.0 240.0 30.0"

set tcp [new Agent/TCP]
$ns color 2 Red
$tcp set class_ 2
set sink [new Agent/TCPSink]
$ns attach-agent $node(0) $tcp
$ns attach-agent $node(1) $sink
$ns connect $tcp $sink
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ns at 0.5 "$ftp start"

for {set i 0} {$i<2} {incr i} {
	$ns at 6.0 "$node($i) reset"
}

$ns at 6.0 "finish"
$ns at 6.01 "puts \"NS EXITING...\";$ns half"

puts "Starting Simulator..."
$ns run
