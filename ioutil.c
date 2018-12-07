/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
            
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <fcntl.h>
            
#include "unp.h"
    
void err_sys(char* msg){
    printf("%s\n",msg);
    exit(EXIT_FAILURE);
}   
    
    
ssize_t Write(int fd, void* buf, size_t n){
    return writen(fd,buf,n);    
}   

ssize_t readn(int fd, void* buf, size_t n){
    size_t nleft=n;
    ssize_t nread=0;
    char *ptr=buf;
    while(nleft>0){
        if((nread=read(fd,ptr,nleft))<0){
            if(errno==EINTR){
                nread=0;
            }else{
                return -1;
            }
        }else if(nread==0){
            break;
        }   
        nleft-=nread;
        ptr+=nread;
    }
    
    return n-nleft;
}

ssize_t writen(int fd,const void* buf, size_t n){
    ssize_t nleft=n;
    ssize_t nwrite=0;
    
    
    const char *ptr=buf;
    while(nleft>0){
        if((nwrite=write(fd,ptr,nleft))<0){
            if(errno==EINTR){
                nwrite=0;
            }else{
                return -1;
            }
        }else if(nwrite==0){
           return -1; 
        }
        nleft-=nwrite;
        ptr+=nwrite;
    }
    return n-nleft;
}

static char buff[1024];
static int cnt=0;
static char *ptr=buff;

char my_read(int fd){
    int nread;
    while(1){
        if(cnt==0){
            nread=read(fd,buff,1024-cnt);
            if(nread<=0){
                return -1;
            }else{
                cnt+=nread;
                ptr=buff;
            }
        }else if(cnt >0){
            cnt--;
            return *ptr++;
        }
    }
}


ssize_t readline(int fd, void* buf, size_t size){
    
    char c;
    char *ptr=buf;
    for(int i=0;i<size;i++){
        c=my_read(fd);
        if(c<0){
            return -1;
        }else if(c=='\n'||c==0){
            *ptr=0;
            return i;
        }
        *ptr++=c;
    }
    *ptr=0;
    return -1;
}

ssize_t Read(int fd, void* buf, size_t maxn){
    int n;
    
again:if((n=read(fd,buf,maxn))<0){
        if(errno==EINTR)
            goto again;
        else
            return -1;
    }    
    return n;
}

ssize_t str_cli(FILE* fd, int socketfd){
    int all=0;
    int maxfdn;
    fd_set rset;
    char sendline[MAX_LINE],revrline[MAX_LINE];
    int stdineof=0;
    FD_ZERO(&rset);
    
    int nread=0;
    while(1){
        FD_SET(fileno(fd),&rset);
        FD_SET(socketfd,&rset);
        
        maxfdn=MAX(socketfd,fileno(fd))+1;
        Select(maxfdn,&rset,NULL,NULL,NULL);
        
        if(FD_ISSET(socketfd,&rset)){
            if((nread=Read(socketfd,sendline,MAX_LINE-1)) <= 0){
                if(nread==-1){
                    err_sys("server terminated prematurely");
                }else{
                    if(stdineof==1){
                        return all;
                    }else{
                        err_sys("server terminated prematurely");
                    }
                }
               
            }
            all+=nread;
            writen(fileno(stdout),sendline,nread);
            printf("\n");
        }
        
        if(FD_ISSET(fileno(fd),&rset)){
            if((nread=Read(fileno(fd),sendline,MAX_LINE-1))<=0){
                if(nread==0){
                    stdineof=1;
                    shutdown(socketfd,SHUT_WR);
                    FD_CLR(fileno(fd),&rset);
                    
                }else{
                    err_sys("server terminated prematurely");
                }
                continue;
            }
            writen(socketfd,sendline,strlen(sendline));
        }
    }
    
    return all;
}





