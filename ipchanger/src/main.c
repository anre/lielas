#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INTERFACE_PATH  "/etc/network/interfaces" 
#define RESOLV_CONF    "/etc/resolv.conf"

#define BUF_SIZE 500

void print_help();
void get_settings();
void set_static(char *adr, char *mask, char *gw);
void set_dhcp();
int save_backup();
int restore_backup();

/********************************************************************************************************************************
*   int main(int argc, char *argv[])
 ********************************************************************************************************************************/

int main(int argc, char *argv[]){

  if(argc < 2){
    print_help();
    exit(0);
  }
  
  if(!strncmp(argv[1], "get", 3)){
    printf("get network settings\n");
    get_settings();
  }else if(!strncmp(argv[1], "set", 3)){
    printf("set network settings\n");
    if(!strncmp(argv[2], "dhcp", 4)){
      set_dhcp();
      if(system("/sbin/ifdown eth0")){
        exit(-1);
      }
      if(system("/sbin/ifup eth0")){
        exit(-1);
      }
    }else if(!strncmp(argv[2], "static", 6)){
      set_static(argv[3], argv[4], argv[5]);
      if(system("/sbin/ifdown eth0")){
        exit(-1);
      }
      if(system("/sbin/ifup eth0")){
        exit(-1);
      }
    }else{
      exit(-1);
    }
  }else if(!strncmp(argv[1], "restore", 7)){
    printf("restoring network settings\n");
    if(restore_backup()){
      exit(-1);
    }
    if(system("/sbin/ifdown eth0")){
      exit(-1);
    }
    if(system("/sbin/ifup eth0")){
      exit(-1);
    }
  }else{
    print_help();
  }
  
  return 0;
}

/********************************************************************************************************************************
*   void print_help()
 ********************************************************************************************************************************/

void print_help(){
  printf("Usage:\n");
  printf("ipchanger get\n");
  printf("...returns current ip\n");
  printf("\n");
  printf("ipchanger set dhcp\n");
  printf("...sets eth0 to dhcp\n");
  printf("\n");
  printf("ipchanger set static address netmask gateway\n");
  printf("...sets eth0 to static ip\n");
  printf("\n");
  printf("ipchanger restore\n");
  printf("...restores settings from interfaces.old\n");
  printf("\n");
}

/********************************************************************************************************************************
*   void get_settings()
 ********************************************************************************************************************************/
 
void get_settings(){
  FILE *file;
  char buf[BUF_SIZE];
  char *ptr;
  char adr[BUF_SIZE];
  char mask[BUF_SIZE];
  char gw[BUF_SIZE];
  int i;
  
  file = fopen( INTERFACE_PATH, "r");
  if(file == NULL){
    exit(-1);
  }
  
  while(fgets(buf, BUF_SIZE, file) != NULL){
    ptr = strstr(buf, "iface eth0 inet ");
    if(ptr){  //interface found, get type
      ptr = strstr(buf, "dhcp");
      if(ptr){
        printf("dhcp\n");
        exit(0);
      }else{
        ptr = strstr(buf, "static");
        if(ptr){
          printf("static\n");
        }else{
          printf("unknown\n");
          exit(-1);
        }
      }
      
      //type static, get address, netmassk and gateway
      //ignore next line: pre-up ...
      if(fgets(buf, BUF_SIZE, file) == NULL){
        exit(0);
      }
      if(fgets(buf, BUF_SIZE, file) == NULL){
        exit(0);
      }
      
      //get address
      ptr = strstr(buf, "address");
      if(!ptr){
        exit(-1);
      }
      
      ptr += sizeof("address");
      
      for(i=0; i  < BUF_SIZE; i++){
        if(ptr[i] == ' ' || ptr[i] == '\n' || ptr[i] == 0){
          break;
        }
        adr[i] = ptr[i];
      }
      adr[i] = 0;
      printf("%s\n", adr);
      
      //get netmask
      if(fgets(buf, BUF_SIZE, file) == NULL){
        exit(0);
      }
      
      ptr = strstr(buf, "netmask");
      if(!ptr){
        exit(-1);
      }
      
      ptr += sizeof("netmask");
      
      for(i=0; i  < BUF_SIZE; i++){
        if(ptr[i] == ' ' || ptr[i] == '\n' || ptr[i] == 0){
          break;
        }
        mask[i] = ptr[i];
      }
      mask[i] = 0;
      printf("%s\n", mask);
      
      //get gateway
      if(fgets(buf, BUF_SIZE, file) == NULL){
        exit(0);
      }
      
      ptr = strstr(buf, "gateway");
      if(!ptr){
        exit(-1);
      }
      
      ptr += sizeof("gateway");
      
      for(i=0; i  < BUF_SIZE; i++){
        if(ptr[i] == ' ' || ptr[i] == '\n' || ptr[i] == 0){
          break;
        }
        gw[i] = ptr[i];
      }
      gw[i] = 0;
      printf("%s\n", gw);
      
      exit(0);
    }
  }
  fclose(file);
}

/********************************************************************************************************************************
*   void set_dhcp()
 ********************************************************************************************************************************/
void set_dhcp(){
  FILE *file;
  
  if(save_backup()){
    exit(-1);
  }
  
  file = fopen( INTERFACE_PATH, "w");
  if(file == NULL){
    exit(-1);
  }  
  
  fprintf(file, "auto lo\n");
  fprintf(file, "iface lo inet loopback\n");
  fprintf(file, "\n");
  fprintf(file, "# The \"pre-up\" lines are to stop a WEMAC NIC from being configured in case\n");
  fprintf(file, "# there are other non-WEMAC NICs in the system. This is necessary to avoid\n");
  fprintf(file, "# using WEMAC on devices with no built-in Ethernet like the MK802.\n");
  fprintf(file, "\n");
  fprintf(file, "# If you use a device with built-in Ethernet and also use an external NIC\n");
  fprintf(file, "# (e.g. for a router usage scenario), feel free to remove these pre-up lines\n");
  fprintf(file, "# so that both NICs are used.\n");
  fprintf(file, "\n");
  fprintf(file, "# If you use an MK802 as WiFi-only (setting up wlan0 and disconnecting\n");
  fprintf(file, "# USB Ethernet), remove both eth0/eth1 sections entirely.\n");
  fprintf(file, "\n");
  fprintf(file, "auto eth0\n");
  fprintf(file, "iface eth0 inet dhcp\n");
  fprintf(file, "  pre-up /usr/local/bin/eni-pre-up.sh eth0\n");
  fprintf(file, "iface eth0 inet6 static\n");
  fprintf(file, "	 address 2001:15c0:666d:100::1\n");
  fprintf(file, "	 netmask 64\n");
  fprintf(file, "\n");
  fprintf(file, "auto eth1\n");
  fprintf(file, "iface eth1 inet dhcp\n");
  fprintf(file, "  pre-up /usr/local/bin/eni-pre-up.sh eth1\n");
  fprintf(file, "\n");
  
  fclose(file);
}

/********************************************************************************************************************************
*   void set_static(char *adr, char *mask, char *gw)
 ********************************************************************************************************************************/
void set_static(char *adr, char *mask, char *gw){
  FILE *file;
  
  if(save_backup()){
    exit(-1);
  }
  
  file = fopen( INTERFACE_PATH, "w");
  if(file == NULL){
    exit(-1);
  }  
  
  fprintf(file, "auto lo\n");
  fprintf(file, "iface lo inet loopback\n");
  fprintf(file, "\n");
  fprintf(file, "# The \"pre-up\" lines are to stop a WEMAC NIC from being configured in case\n");
  fprintf(file, "# there are other non-WEMAC NICs in the system. This is necessary to avoid\n");
  fprintf(file, "# using WEMAC on devices with no built-in Ethernet like the MK802.\n");
  fprintf(file, "\n");
  fprintf(file, "# If you use a device with built-in Ethernet and also use an external NIC\n");
  fprintf(file, "# (e.g. for a router usage scenario), feel free to remove these pre-up lines\n");
  fprintf(file, "# so that both NICs are used.\n");
  fprintf(file, "\n");
  fprintf(file, "# If you use an MK802 as WiFi-only (setting up wlan0 and disconnecting\n");
  fprintf(file, "# USB Ethernet), remove both eth0/eth1 sections entirely.\n");
  fprintf(file, "\n");
  fprintf(file, "auto eth0\n");
  fprintf(file, "iface eth0 inet static\n");
  fprintf(file, "  pre-up /usr/local/bin/eni-pre-up.sh eth0\n");
  fprintf(file, "  address %s\n", adr);
  fprintf(file, "  netmask %s\n", mask);
  fprintf(file, "  gateway %s\n", gw);
  fprintf(file, "iface eth0 inet6 static\n");
  fprintf(file, "	 address 2001:15c0:666d:100::1\n");
  fprintf(file, "	 netmask 64\n");
  fprintf(file, "\n");
  fprintf(file, "auto eth1\n");
  fprintf(file, "iface eth1 inet dhcp\n");
  fprintf(file, "  pre-up /usr/local/bin/eni-pre-up.sh eth1\n");
  fprintf(file, "\n");
  
  fclose(file);
  
  //set nameserver in resolve.conf
  file = fopen( RESOLV_CONF, "w");
  if(file == NULL){
    exit(-1);
  }  
  fprintf(file, "nameserver %s\n", gw);
  fclose(file);
}

/********************************************************************************************************************************
*   int save_backup()
 ********************************************************************************************************************************/
int save_backup(){
  char cmd[BUF_SIZE];
  
  sprintf(cmd, "cp %s %s.bak", INTERFACE_PATH, INTERFACE_PATH);
  
  return system(cmd);
}

/********************************************************************************************************************************
*   int restore_backup()
 ********************************************************************************************************************************/
int restore_backup(){
  char cmd[BUF_SIZE];
  
  sprintf(cmd, "cp %s.bak %s", INTERFACE_PATH, INTERFACE_PATH);
  
  return system(cmd);
 }
 

