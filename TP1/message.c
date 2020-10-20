#include "message.h"
#include "protocol.h"
#include "state_machine.h"

int flag=1, conta=1;

int write_supervision_message(int fd, char cc_value){
    char trama[SUPERVISION_TRAMA_SIZE];
    trama[FLAGI_POSTION] = F;
	trama[ADRESS_POSITION] = AREC;
	trama[CC_POSITION] = cc_value;
	trama[PC_POSITION] = (AREC) ^ (cc_value);
	trama[FLAGF_POSTION] = F;
	trama[SUPERVISION_TRAMA_SIZE - 1] = '\0';

    return write(fd,trama,SUPERVISION_TRAMA_SIZE);
}

int write_info_message(int fd,char * data,int data_size, char cc_value){
	if(data_size==0) return -1;

	int trama_size = INFO_SIZE_MSG(data_size);

	char* trama = malloc(trama_size + 1);

    trama[FLAGI_POSTION] = F;
	trama[ADRESS_POSITION] = AREC;
	trama[CC_POSITION] = CC_INFO_MSG(cc_value); //0S00000
	trama[PC_POSITION] = (AREC) ^ CC_INFO_MSG(cc_value);
	trama[trama_size - 2] = buildBCC2(data, data_size);
	trama[trama_size - 1] = F;
	trama[trama_size] = '\0';

	memcpy(trama + DATA_INF_BYTE,data,data_size);

	return write(fd,trama,INFO_SIZE_MSG(data_size));
}

int write_supervision_message_retry(int fd,char cc_value){
	char received[SUPERVISION_TRAMA_SIZE];

	memset(received, 0, strlen(received));

	int rd;
	int success = FALSE;

	while(conta <= WRITE_NUM_TRIES && !success){
		if(flag){
			alarm(RESEND_DELAY);
			flag=0;

			/* writing */ 
			if(write_supervision_message(fd,cc_value)==-1){
				printf("Error writing supervision message\n");
			}

			/* read message back */
			rd = readMessage(fd, received);
			if (rd != sizeof(received)) 
				success = FALSE;
			else{
				success = TRUE;
				reset_alarm();
			} 
		}
	}
	if (success == TRUE){
		printSupervisionMessage(received);
		return 1;
	}
	return 0;
}

int write_inform_message_retry(int fd,char cc_value,int dataSize,char * buffer){
	printf("BEGGINING WRTE RETRY\n");
	char received[INFO_SIZE_MSG(dataSize)];


	memset(received, 0, strlen(received));

	int rd;
	int success = FALSE;

	while(conta <= WRITE_NUM_TRIES && !success){
		if(flag){
			alarm(RESEND_DELAY);
			flag=0;

			/* writing */ 
			if(write_info_message(fd,buffer,dataSize,cc_value)==-1){
				printf("Error writing Inform message\n");
			}

			/* read message back */
			rd = readMessage(fd, received);
			if (rd != sizeof(received)) 
				success = FALSE;
			else{
				success = TRUE;
				reset_alarm();
			} 
		}
	}


	if (success == TRUE){
		printf("END WRTE RETRY SUCCESS\n");

		printInformMessage(received,dataSize);
		return 1;
	}
	printf("END WRTE RETRY FAIL\n");

	return 0;
}


int readSupervisionMessage(int fd){
	char trama[SUPERVISION_TRAMA_SIZE];
	int check = readMessage(fd, trama);

	if(check==-1){
		printf("Error reading message\n");
		return 0;
	} 
	if(check != sizeof(trama)){
		return 0;
	}
	printSupervisionMessage(trama);	
	return 1;

}

void printSupervisionMessage(char * trama){
	printf("Supervision message recieved correctly\n");
	printf("F: %04x\nA: %04x\n", trama[0], trama[1]);
	printf("C: %04x\n", trama[2]);
	printf("BCC: %04x\n", trama[3]);
}

void printInformMessage(char * trama,int dataSize){
	printf("Inform message recieved correctly\n");
	printf("F: %04x\nA: %04x\n", trama[0], trama[1]);
	printf("C: %04x\n", trama[2]);
	printf("BCC: %04x\n", trama[3]);
	printf("Data: \n");
	for (size_t i = 4; i < dataSize+4; i++)
		printf("%c", trama[i]);
	printf("\n");
	printf("BCC2: %04x\n", trama[dataSize+5]);
	printf("F: %04x\n", trama[dataSize+6]);
}

void atende(int signo){
	printf("alarme # %d\n", conta);
	flag=1;
	conta++;
}

void install_alarm(){
	struct sigaction action;
    action.sa_handler = atende;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGALRM,&action,NULL) < 0){
        fprintf(stderr,"Unable to install SIGALRM handler\n");
        exit(1);
    }
}

void reset_alarm(){
	flag = 1;
	conta = 1;
	alarm(0);
}

int readMessage(int fd,char * buffer){  
	char r;
	int finished = FALSE, rd, pos = 0;
	while (!finished){
		//printf("State machine: %d \n", getStateMachine());
		rd = read(fd, &r, 1);
		if (rd <= 0){
			return -1;
		}
		else if (getStateMachine() == STOP){
			finished = TRUE;
		}

		handleState(r,0);
		buffer[pos++] = r;
	}
	return pos;
}



char buildBCC2(char * data, int data_size) {
	char xor = data[0] ^ data[1];
	for (int i = 2; i < data_size; i++) {
		xor = xor ^ data[i];
	}
	return xor;
}
