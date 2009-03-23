打算使用multi-homing綁在一個socket上
這一個例子是用來測試
#include <netinet/sctp.h>
int sctp_bindx(int sockfd,const struct sockaddr *addrs,int addrcnt,int flags);
returns:0 if OK,-1 on error

server端
後面加一個本機端的ip，讓socket可以在上面服務,例:
./server 127.0.0.1

另外,server端一定會bind成功,若server端輸入的ip是錯誤的,則系統會自動bind本機端的所有ip
若輸入是正確的，就只會bind所指定的ip

client端:
在後面加入server端的ip
./client 127.0.0.1
輸入
[0]Hello
[1]World

也可以測式失敗的例子:
先透過ifconfig得知host有幾個ip
假設有192.168.20.100與127.0.0.1
server端:
./server 127.0.0.1

client端:
./client 192.168.20.100
[1]Hello
則會出現錯誤哩

[新增]
當client端連上server端之後,server端會列印出local綁在socket上的個數與client端綁在socket上的個數
