// control packet
#define CTRL_SIZE_OCTET             0
#define CTRL_NAME_OCTET             1
#define CTRL_SIZE_T_IDX             1
#define CTRL_SIZE_L_IDX             2
#define CTRL_SIZE_V_IDX             3
#define CTRL_NAME_T_IDX             7
#define CTRL_NAME_L_IDX             8
#define CTRL_NAME_V_IDX             9
#define CTRL_PACKET_SIZE(fname_len) sizeof(u_int) + fname_len + 5
#define DATA_PACKET_SIZE(data_len)  data_len + 4
#define DATA_PACKET_MAX_SIZE        4096

// control 
#define PACKET_CTRL_IDX        0
#define PACKET_CTRL_DATA       1
#define PACKET_CTRL_START      2
#define PACKET_CTRL_END        3

typedef struct{
    char * control_packet;
    int size;
} ctrl_packet;



int sendFile(char * port_num,char * filename);
int retrieveFile(char * port_num);