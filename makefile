
all:
	g++ -o UdpServer MyServer.cc -lpthread -std=c++17
	g++ -o UdpClinet UdpClinet.cc -lpthread -std=c++17


.PHONY: clean
clean:
	rm -f UdpServer UdpClinet
