all: client server

socket_init: socket_init.cpp
	g++ -c socket_init.cpp -o socket_init
udp_socket: udp_socket.cpp socket_init
	g++ -c udp_socket.cpp -o udp_socket
myTypes: myTypes.cpp
	g++ -c myTypes.cpp -o myTypes
protocol_server: protocol_server.cpp udp_socket myTypes
	g++ -c protocol_server.cpp -o protocol_server -lpthread
protocol_client: protocol_client.cpp udp_socket myTypes
	g++ -c protocol_client.cpp -o protocol_client -lpthread

client: client.cpp protocol_client udp_socket socket_init
	g++ client.cpp protocol_client udp_socket socket_init myTypes -o client -lpthread
server: server.cpp protocol_server
	g++ server.cpp protocol_server udp_socket socket_init myTypes -o server -lpthread
