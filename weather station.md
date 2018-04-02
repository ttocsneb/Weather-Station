Weather Station Pseudocode
=======

## Weather Station

> Power up
	
setup rf24
load settings
	increase number of boots by 1
	load update time
	load loop time
init sensors

> Loop

update sensors
wait loop time
check if update time has passed
	power up rf24
	send weather data
	check for commands
	power down rf24

> commands

change setting
	update time
	loop time
	number of restarts
	
	
get status
	uptime in seconds
	number of resets
	voltage
	charging
	lowest voltage in last 24 hours
	highest voltage in last 24 hours
	lowest voltage since last reset
	highest voltage since last reset

> sensors

Wind: rpm
rain: centi inches
temperature: deci celsius
humidity: 0-99
pressure: centi inches

## Base Station

> Start

setup rf24
start listening
wait until data is received


> Loop

Wait until rf24 data is available
read weather data
if there are commands in the queue
	switch to write mode
	while there are commands in the queue
		execute command
	switch to read mode

