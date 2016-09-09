/*-------------------------------------------------------------*/
/* Exemplo Socket Raw - Captura pacotes recebidos na interface */
/*-------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

/* Diretorios: net, netinet, linux contem os includes que descrevem */
/* as estruturas de dados do header dos protocolos   	  	        */

#include <net/if.h>  //estrutura ifr
#include <netinet/ether.h> //header ethernet
#include <netinet/in.h> //definicao de protocolos
#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP

#include <netinet/in_systm.h> //tipos de dados

#define BUFFSIZE 1518

// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

unsigned char buff1[BUFFSIZE]; // buffer de recepcao

int sockd;
int on;
struct ifreq ifr;

void print_hex(uint8_t *s, size_t len) {
    for(int i = 0; i < len; i++) {
        printf("%02x:", s[i]);
    }
}

int main(int argc,char *argv[])
{
	printf("argc: %d\n",argc);
	if(argc < 2){
		printf("Informe uma interface de rede.\n");
		exit(1);
	}
	
	char* interface = argv[1];
	
	printf("interface: %s\n",interface);

    /* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
    /* De um "man" para ver os parametros.*/
    /* htons: converte um short (2-byte) integer para standard network byte order. */
    if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
       printf("Erro na criacao do socket.\n");
       exit(1);
    }

	// O procedimento abaixo eh utilizado para "setar" a interface em modo promiscuo
	strcpy(ifr.ifr_name, interface);
	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
		printf("erro no ioctl!");
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);
	
    
    printf("ETHERTYPE_IP: %x\n",ETHERTYPE_IP);
    printf("ETHERTYPE_ARP: %x\n",ETHERTYPE_ARP);
    printf("ETHERTYPE_IPV6: %x\n",ETHERTYPE_IPV6);
	// recepcao de pacotes
	while (1) {
   		recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
		// impress�o do conteudo - exemplo Endereco Destino e Endereco Origem
        struct ether_header *test = (struct ether_header *) buff1;
        printf("test MAC dst: ");
        print_hex(test->ether_dhost, 6);
        printf("\n");            
        printf("test MAC src: ");
        print_hex(test->ether_shost, 6);
        printf("\n");
        printf("test MAC typ: %u\n",test->ether_type);
        if((unsigned short)ETHERTYPE_IP==(unsigned short)test->ether_type){
            printf("BIIIRRLLLL\n");
        }
		//printf("MAC Destino: %x:%x:%x:%x:%x:%x \n", buff1[0],buff1[1],buff1[2],buff1[3],buff1[4],buff1[5]);
		//printf("MAC Origem:  %x:%x:%x:%x:%x:%x \n", buff1[6],buff1[7],buff1[8],buff1[9],buff1[10],buff1[11]);
		printf("Tipo:        %02x%02x\n",buff1[12],buff1[13]);
	}
}
