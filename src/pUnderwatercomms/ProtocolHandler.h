#include <string>
#include <vector>
#include <cstdint>

struct AcousticMessage {
    uint8_t type;            // 业务类型 (1)
    uint32_t checksum;       // 校验和 (暂时填 0)
    uint32_t length;         // 数据长度 (从 "业务类型" 到 "数据" 所有字节)
    uint8_t node_id;         // 目的节点编号
    uint8_t mode;            // 发送模式
    uint8_t retransmit;      // 是否重传
    uint8_t control_mode;    // 1=自动模式, 0=手动模式
    uint32_t send_time;      // 发送时间 (Linux时间戳)
    std::vector<uint8_t> user_data;
};

class ProtocolHandler {
public:
    std::string encodeMessage(const AcousticMessage& msg);
    AcousticMessage decodeMessage(const std::string& hex_data);
};
