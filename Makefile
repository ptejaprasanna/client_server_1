all:	tcpclient tcpserver

tcpclient: tcpclient.cpp;
	g++ tcpclient.cpp -o tcpclient

tcpserver: tcpserver.cpp;
	g++ tcpserver.cpp -o tcpserver

clean:
	rm tcpclient tcpserver
