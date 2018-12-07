/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <bits/errno.h>
#include <fcntl.h>
#include <string.h>
#include "unp.h"

int Socket(int af, int type, int prtocol){
    int n=0;
    if((n=socket(af,type,prtocol))<0){
        err_sys("create a socket failed");
    }
    return n;
}

int Bind(int lfd,struct sockaddr* addr, ssize_t len){
    int n=0;
    if((n=bind(lfd,addr,len))<0){
        printf("errno is %s\n",strerror(errno));
        err_sys("scoket bind failed");
    }
    return n;
}

int connect_nonb(int sockfd,const SA* saptr,socklen_t len,int nsec){
    
    int flags=fcntl(sockfd,F_GETFL);
    fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);
    
    int error=0;
    int n=connect(sockfd,saptr,len);
    if(n<0){
        if (errno!=EINPROGRESS){
            return -1;
        }
    }
    if(n==0){
        goto done;
    }    
    
    fd_set rset,wset;
    struct timeval val;
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    
    FD_SET(sockfd,&rset);
    FD_SET(sockfd,&wset);
    
    val.tv_usec=0;
    val.tv_sec=nsec;
    
    if(Select(sockfd+1,&rset,&wset,NULL,nsec?&val:NULL)==0){
        errno=error=ETIMEDOUT;
    }
    
    
    if(FD_ISSET(sockfd,&rset)||FD_ISSET(sockfd,&wset)){
        len=sizeof(error);
        if(getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&error,&len)<0){
        }
    }else{
        err_sys("the select is not set");
    }
    
    
    done:
    fcntl(sockfd,F_SETFL,flags);
    if(error!=0){
        close(sockfd);
        return -1;
    }
    return 0;
}

int Connect(int sfd,struct sockaddr* addr, ssize_t len){
    int n=0;
    if((n=connect(sfd,addr,len))<0){
        err_sys("can't connect to socket");
    }
    return n;
}

int Accept(int lfd,struct sockaddr* addr, socklen_t* len){
    int n=0;
    if((n=accept(lfd,addr,len))<0){
        err_sys("accept a socket failed");
    }
    return n;
}

int Listen(int lfd, int backlog){
    int n=0;
    char *ptr=getenv("LISTENQ");
    if(ptr!=NULL){
        n=listen(lfd,atoi(ptr));
    }else{
        n=listen(lfd,backlog);
    }
    
    if(n<0){
        err_sys("listen failed");
    }
    
    return n;
}

int Select (int __nfds, fd_set *readfds,
		   fd_set *writefds,
		   fd_set *exceptfds,
		   struct timeval *timeout){
    int n;
    
    while((n=select(__nfds,readfds,writefds,exceptfds,timeout))<0){
        if(errno!=EINTR){
            return -1;
        }
    }
   
    return n;
}

ssize_t Recvfrom(int fd, void* buf, size_t n, int flags,struct sockaddr* addr, socklen_t* addrlen){
    ssize_t recvn;
    again:
    if((recvn=recvfrom(fd,buf,n,flags,addr,addrlen))<0){
        if(errno==EINTR){
            goto again;
        }else{
            return -1;
        }
    }
    return recvn;
}

ssize_t Sendto(int fd, void* buf, size_t n, int flags,struct sockaddr* addr, socklen_t addrlen){
    ssize_t sendn;
again:
    if((sendn=sendto(fd,buf,n,flags,addr,addrlen))<0){
        if(errno==EINTR){
            goto again;
        }else{
            return -1;
        }
    }
    return sendn;
}   

int Sctp_recvmsg(int s, void *msg, size_t len, struct sockaddr *from,
		 socklen_t *fromlen, struct sctp_sndrcvinfo *sinfo,
		 int *msg_flags){
    int n;
    again:
    
//    if((n=sctp_recvmsg(s,msg,len,from,fromlen,sinfo,msg_flags))<0){
//        if(errno==EINTR||errno==EAGAIN){
//            goto again;
//        }else{
//            err_sys("sctp err recv");
//        }
//    }
    
    return n;
}

int Sctp_sendmsg(int s, const void *msg, size_t len, struct sockaddr *to,
		 socklen_t tolen, uint32_t ppid, uint32_t flags,
		 uint16_t stream_no, uint32_t timetolive, uint32_t context){
    
//    int n;
//    again:
//    if((n=sctp_sendmsg(s,msg,len,to,tolen,ppid,
//            flags,stream_no,timetolive,context))<0){
//        err_sys("sctp err send");
//        
//    }
//    
    return 0;
}








