/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   unp.h
 * Author: dishfo
 *
 * Created on 2018年10月15日, 下午2:37
 */

#ifndef UNP_H
#define UNP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>

#ifdef __cplusplus
extern "C" {
#endif
#define SA struct sockaddr
#define MAX_LINE 256
    
extern void err_sys(char* msg);    

extern int Socket(int af,int type,int prtocol);
extern int Bind(int lfd,struct sockaddr * addr,ssize_t len);
extern int Connect(int sfd,struct sockaddr *addr,ssize_t len);
extern int Accept(int lfd,struct sockaddr *addr,socklen_t *len);
extern int Listen(int lfd,int backlog);

extern ssize_t Recvfrom(int fd, void * buf, size_t n,
			 int flags, SA *addr,
			 socklen_t * addrlen);

extern ssize_t Sendto(int fd, void * buf, size_t n,
			 int flags, SA *addr,
			 socklen_t  addrlen);


extern char * sock_ntop(struct sockaddr *addr, ssize_t len);
extern ssize_t Read(int fd,void *buf,size_t n);
extern ssize_t readn(int fd,void *buf,size_t n);
extern ssize_t writen(int fd,const void *buf,size_t n);
extern ssize_t readline(int fd,void *buf,size_t size);
extern ssize_t str_cli(FILE *fd,int socketfd);
extern ssize_t Write(int fd,void *buf,size_t n);

extern int Select (int __nfds, fd_set *readfds,
		   fd_set *writefds,
		   fd_set *exceptfds,
		   struct timeval *timeout);

extern void dg_cli(int sockfd, SA * pcliaddr,socklen_t len);
extern void dg_echo(int sockfd, SA * pserveraddr,socklen_t len);
extern int Sctp_recvmsg(int s, void *msg, size_t len, struct sockaddr *from,
		 socklen_t *fromlen, struct sctp_sndrcvinfo *sinfo,
		 int *msg_flags);

extern int Sctp_sendmsg(int s, const void *msg, size_t len, struct sockaddr *to,
		 socklen_t tolen, uint32_t ppid, uint32_t flags,
		 uint16_t stream_no, uint32_t timetolive, uint32_t context);

extern int connect_nonb(int sockfd,const SA* saptr,socklen_t len,int nsec);

#ifdef __cplusplus
}
#endif

#endif /* UNP_H */

