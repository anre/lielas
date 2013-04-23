lielas
======

lielas is an open-source automation software specialized on handling ipv6 dataloggers and presenting data in a webinterface.



Architecture:


*) sql/sql
	SQL-Handling functions
	
*) jsmn/jsmn
	jsmn JSON Parser
	http://zserge.com/jsmn.html
	
*) coap/libcoap
	libcoap header files
	
*) coap/coapserver
	libcoap coap server
	
*) coap/mycoap
	libcoap client for communication with devices
	
*) liegw
	main c file
	
*) settings
	save/load settings to/from config file
	
*) log
	set log-mode ( debug, warn, error)
	set log target (stdout, file, syslog)
	
*) lielas/devicecontainer
	init devicecontainer, load, add, remove, change devices
	
*) lielas/device
	create, delete devices
	add, remove moduls
	
*) lielas/moduls
	create, delete moduls
	add, remove channels
	
*) lielas/channel
	create, delete channels
	
*) lielas/ldb
	lielas database functions
	create, delete Tables
	
*) lielas/lbus
	lielas bus
	handle lbus commands
	
	
	
	
	
	
	
	