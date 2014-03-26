#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"

#define INTERFACE_PATH  "/etc/network/interfaces" 
#define RESOLV_CONF    "/etc/resolv.conf"

#define BUF_SIZE 500

char ver[] = "1.1.0";

void print_help();
void get_settings();
void set_static(char *adr, char *mask, char *gw);
void set_dhcp();
int save_backup();
int restore_backup();
void restart_net_interface();
void set_dns(char *dns1, char *dns2);

/********************************************************************************************************************************
*   int main(int argc, char *argv[])
 ********************************************************************************************************************************/

int main(int argc, char *argv[]){
  
  if(argc < 2){
    print_help();
    exit(0);
  }
  
  if(!strncmp(argv[1], "get", 3)){
    
    write_log("get network settings", LOG_LEVEL_DEBUG);
    
    get_settings();
    
  }else if(!strncmp(argv[1], "set", 3)){
    
    
    if(!strncmp(argv[2], "dhcp", 4)){
      write_log("set network settings dhcp", LOG_LEVEL_DEBUG);
      
      set_dhcp();
      restart_net_interface();
      
    }else if(!strncmp(argv[2], "static", 6)){
      write_log("set network settings static", LOG_LEVEL_DEBUG);
      
      set_static(argv[3], argv[4], argv[5]);
      restart_net_interface();
      
    }else if(!strncmp(argv[2], "dns", 3)){
      write_log("set dns server", LOG_LEVEL_DEBUG);
      
      if( argc == 4 ){
        set_dns(argv[3], NULL);
      }else if(argc == 5){
        set_dns(argv[3], argv[4]);
      }
      
    }else{
      exit(-1);
    }
  }else if(!strncmp(argv[1], "restore", 7)){
    
    write_log("restoring network settings", LOG_LEVEL_DEBUG);
    if(restore_backup()){
      write_log("failed to restore network settings", LOG_LEVEL_ERROR);
      exit(-1);
    }
    restart_net_interface();
    
  }else{
    print_help();
  }
  
  return 0;
}

/********************************************************************************************************************************
*   void print_help()
 ********************************************************************************************************************************/

void print_help(){
  printf("Version: %s\n", ver);
  printf("Usage:\n");
  printf("ipchanger get\n");
  printf("...returns current ip\n");
  printf("\n");
  printf("ipchanger set dhcp\n");
  printf("...sets eth0 to dhcp\n");
  printf("\n");
  printf("ipchanger set static <address> <netmask> <gateway>\n");
  printf("...sets eth0 to static ip\n");
  printf("\n");
  printf("ipchanger set dns <dns1> <dns2>\n");
  printf("...sets dns1 and if given dns2\n");
  printf("\n");
  printf("ipchanger restore\n");
  printf("...restores settings from interfaces.old\n");
  printf("\n");
}

/********************************************************************************************************************************
*   void restart_net_interface()
 ********************************************************************************************************************************/
void restart_net_interface(){
    write_log("resetting eth0", LOG_LEVEL_DEBUG);
    sleep(2);
    if(system("/sbin/ifdown eth0")){
      write_log("failed to shut down eth0", LOG_LEVEL_ERROR);
      exit(-1);
    }
    sleep(5);
    if(system("/sbin/ifup -v eth0")){
      write_log("failed to bring eth0 up", LOG_LEVEL_ERROR);
      exit(-1);
    }
    sleep(2);
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
    write_log("set_dhcp: failed to save backup", LOG_LEVEL_ERROR);
    exit(-1);
  }
  
  file = fopen( INTERFACE_PATH, "w");
  if(file == NULL){
    write_log("set_dhcp: failed to open interface file", LOG_LEVEL_ERROR);
    exit(-1);
  }  
  
  fprintf(file, "auto lo\n");
  fprintf(file, "iface lo inet loopback\n");
  fprintf(file, "\n");
  fprintf(file, "auto eth0\n");
  fprintf(file, "iface eth0 inet dhcp\n");
  fprintf(file, "\n");
  /*fprintf(file, "iface eth0 inet6 static\n");
  fprintf(file, "	 address fd23:557d:21e0:1::334c\n");
  fprintf(file, "	 netmask 64\n");
  fprintf(file, "\n");*/
  
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
    write_log("set_static: failed to open interface file", LOG_LEVEL_ERROR);
    exit(-1);
  }  
  
  fprintf(file, "auto lo\n");
  fprintf(file, "iface lo inet loopback\n");
  fprintf(file, "\n");
  fprintf(file, "auto eth0\n");
  fprintf(file, "iface eth0 inet static\n");
  fprintf(file, "  address %s\n", adr);
  fprintf(file, "  netmask %s\n", mask);
  fprintf(file, "  gateway %s\n", gw);
  /*fprintf(file, "iface eth0 inet6 static\n");
  fprintf(file, "	 address fd23:557d:21e0:1::334c\n");
  fprintf(file, "	 netmask 64\n");
  fprintf(file, "\n");*/
  
  fclose(file);
  
}
/********************************************************************************************************************************
*   void set_dns(char *dns1, char *dns1)
 ********************************************************************************************************************************/
void set_dns(char *dns1, char *dns2){
  FILE *file;
    
  file = fopen( RESOLV_CONF, "w");
  if(file == NULL){
    write_log("set_dns: failed to open resolv.conf", LOG_LEVEL_ERROR);
    exit(-1);
  }  
  if(dns1 != NULL){
    fprintf(file, "nameserver %s\n", dns1);
  }
  if(dns2 != NULL){
    fprintf(file, "nameserver %s\n", dns2);
  }
  
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
 

