#ifndef PROTOCOLHANDLER_H
#define PROTOCOLHANDLER_H

#include <string>
#include <vector>
#include <cstdint>
#include <boost/crc.hpp>

struct AcousticMessage {
    uint8_t type;             // 业务类型 (1字节)
    uint32_t checksum;        // CRC32 校验和 (4字节)
    uint32_t length;          // 数据长度 (4字节)
    uint8_t node_id;          // 目的节点编号 (1字节)
    uint8_t mode;             // 发送模式 (1字节)
    uint8_t retransmit;       // 是否重传 (1字节)
    uint8_t control_mode;     // 控制模式 (1字节)
    uint32_t send_time;       // 发送时间戳 (4字节)
    std::vector<uint8_t> user_data; // 用户数据 (变长)
};

class ProtocolHandler {
public:
    std::string encodeMessage(const AcousticMessage& msg);
    AcousticMessage decodeMessage(const std::string& hex_data);

private:
    static uint32_t calculateChecksum(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> hexStringToBytes(const std::string& hex);
    static std::string bytesToHexString(const std::vector<uint8_t>& bytes);
};

#endif