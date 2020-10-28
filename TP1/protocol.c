#include "protocol.h"
#include "connection.h"
#include "message.h"
#include "state_machine.h"
#include "data_stuffing.h"

conn_type connection;

int llopen(char* port, conn_type connection_type) {
	link_layer layer;
	strcpy(layer.port,port);
	layer.baud_rate = BAUDRATE;
  	layer.num_transmissions = NUM_TRANSMISSIONS;
  	layer.timeout = LAYER_TIMEOUT;
	connection = connection_type;
	  
	int fd = open_connection(layer);
	if(connection_type == EMISSOR){
		printf("Writing SET message\n");

		if(write_supervision_message_retry(fd,SET) == -1){
			printf("Error establishing connection\n");
		}
	}else if (connection_type == RECEPTOR){
		if(readSupervisionMessage(fd) == -1){
			printf("Error reading SET message\n");
		}
		if(write_supervision_message(fd,UA) == -1){
			printf("Error writing UA\n");
		}
	}
	return fd;
}

int llclose(int fd) {
	if(connection == EMISSOR){
		printf("Writing DISC message\n");

		if(write_supervision_message_retry(fd,DISC) == -1){
			printf("Error establishing connection\n");
		}
		if(write_supervision_message(fd,UA) == -1){
			printf("Error writing UA\n");
		}
	}
	else if (connection == RECEPTOR) {
		int buffer_size = 0;
		char* temp = readMessage(fd, &buffer_size, 1);

		if (temp[CTRL_POS] == DISC) {
			if (write_supervision_message(fd, DISC) == -1){
				printf("LLREAD: error writing DISC message back\n");
				return -2;
			}
		}
		else{
			printf("Error receiving DISC message\n");
		}

		free(temp);
		temp = readMessage(fd, &buffer_size, 1);

		if (temp == NULL || buffer_size == 0){
			printf("LLREAD: error reading UA message after sending DISC\n");
			return -1;
		}

		if (temp[CTRL_POS] == UA) {
			return 0;
		}
		else {
			printf("LLREAD: wrong message after sending DISC\n");
			return -1;
		}
	}

    return close_connection(fd);
}

int llwrite(int fd, char* buffer, int length) {
	data_stuff stuffedData = stuffData(buffer,length); 

	return write_inform_message_retry(fd, 1, stuffedData.data_size, stuffedData.data);
	
	//return write_inform_message_retry(fd, 1 ,length, buffer);
}

int llread(int fd, char* buffer) {
	int buffer_size = 0;

	char* temp = readMessage(fd, &buffer_size, 1);
	data_stuff unstuffedData = unstuffData(temp,buffer_size);
	memcpy(buffer, unstuffedData.data + DATA_INF_BYTE, unstuffedData.data_size - 7);

	/*printf("temp\n");
	for(int i = 0; i < buffer_size; i++) {
		printf("%c", temp[i]);
	}

	printf("\n\n");

	printf("buffer\n");
	for(int i = 0; i < buffer_size - 6; i++) {
		printf("%c", buffer[i]);
	}

	printf("\n\n");*/

	//printDataInfoMsg(buffer,buffer_size);

	if (temp == NULL || buffer_size == 0){
		printf("LLREAD: error reading message\n");
		return -1;
	}

	if (write_supervision_message(fd, RR(1)) == -1){
		printf("LLREAD: error writing message back\n");
		return -1;
	}

	return buffer_size - 7;
}
