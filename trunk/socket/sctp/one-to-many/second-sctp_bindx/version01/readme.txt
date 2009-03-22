打算使用multi-homing綁在一個socket上

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
