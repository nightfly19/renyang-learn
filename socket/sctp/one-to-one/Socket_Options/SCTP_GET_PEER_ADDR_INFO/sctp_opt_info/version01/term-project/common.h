// 資料大量 不需即時性 需要資料完整
#define DATA_STREAM 1
// 資料小 需即時性 不需要資料完整 權重最高
#define VOIP_STREAM 2
// 資料大 需即時性 不需要資料完整 權重低
#define VOICE_STREAM 3
// 資料小 不需即時性 需要資料完整
#define COMMAND_STREAM 4

#define MAX_BUFFER 1024
#define MY_PORT_NUM 10000
