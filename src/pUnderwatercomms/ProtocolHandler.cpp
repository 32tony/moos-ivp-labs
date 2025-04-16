#include "ProtocolHandler.h"
#include <sstream>
#include <iomanip>
#include <cstring>

std::string ProtocolHandler::encodeMessage(const AcousticMessage& msg) {
    std::ostringstream oss;
    oss << "@";

    // 1. 构建正文部分（从 type 开始）
    std::ostringstream body;

    // 业务类型 (1 字节)
    body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.type);

    // 校验和 (4 字节，占位)
    body << "00000000";

    // 占位：数据长度 (4 字节)，稍后回填
    std::streampos len_pos = body.tellp();
    body << "00000000";

    // 固定字段（1 字节 * 4 + 4 字节）
    body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.node_id);
    body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.mode);
    body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.retransmit);
    body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.control_mode);
    body << std::hex << std::setw(8) << std::setfill('0') << static_cast<uint32_t>(msg.send_time);

    // 添加变长 userdata（如 nav_lat/nav_long 已提前打包进 userdata）
    for (uint8_t byte : msg.user_data) {
        body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }

    // 2. 计算数据长度并回填（从 type 到最后一个 userdata 字节）
    std::string body_str = body.str();
    size_t byte_len = body_str.length() / 2;

    // 构造小端序长度字符串
uint32_t len_val = static_cast<uint32_t>(byte_len);
std::ostringstream len_ss;
len_ss << std::hex << std::setw(2) << std::setfill('0') << ((len_val >> 0) & 0xFF);
len_ss << std::hex << std::setw(2) << std::setfill('0') << ((len_val >> 8) & 0xFF);
len_ss << std::hex << std::setw(2) << std::setfill('0') << ((len_val >> 16) & 0xFF);
len_ss << std::hex << std::setw(2) << std::setfill('0') << ((len_val >> 24) & 0xFF);

// 替换正文中的长度字段（streampos -> index）
body_str.replace(static_cast<size_t>(len_pos), 8, len_ss.str());

    // 3. 拼接完整消息
    oss << body_str;
    oss << "#";
    
    return oss.str();
}

AcousticMessage ProtocolHandler::decodeMessage(const std::string& hex_data) {
    AcousticMessage msg = {};

    if (hex_data.front() != '@' || hex_data.back() != '#') {
        throw std::runtime_error("Invalid message format");
    }
}