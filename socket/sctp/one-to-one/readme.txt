server端
./serv

client端
./clnt

問題:當連續執行client，會出現
「getsockopt error: Invalid argument」
的錯誤訊息，不知道是不是因為不能同時使用那麼多個system call
