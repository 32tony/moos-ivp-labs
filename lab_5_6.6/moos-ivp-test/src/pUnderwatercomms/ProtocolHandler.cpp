#include "ProtocolHandler.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

//------------------------------------------------------------------
// 编码消息
std::string ProtocolHandler::encodeMessage(const AcousticMessage& msg) {
    std::ostringstream oss;
    oss << "@";  // 起始符

    // 1. 构建主体部分（包含占位符）
    std::ostringstream body;
    body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.type);
    body << "00000000";  // 校验和占位符 (4字节)
    body << "00000000";  // 长度占位符 (4字节)

    // 固定字段
    body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.node_id);
    body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.mode);
    body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.retransmit);
    body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg.control_mode);
    body << std::hex << std::setw(8) << std::setfill('0') << msg.send_time;

    // 用户数据
    for (uint8_t byte : msg.user_data) {
        body << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }

    // 2. 计算数据长度（从 type 到 user_data 的字节数）
    std::string body_str = body.str();
    size_t byte_len = (body_str.length() - 8 - 8) / 2;  // 减去校验和和长度占位符
    byte_len += 4 + 4;  // 补充校验和和长度字段自身

    // 生成小端序长度
    std::ostringstream len_ss;
    len_ss << std::hex << std::setw(2) << std::setfill('0') << ((byte_len >> 0) & 0xFF)
           << std::hex << std::setw(2) << std::setfill('0') << ((byte_len >> 8) & 0xFF)
           << std::hex << std::setw(2) << std::setfill('0') << ((byte_len >> 16) & 0xFF)
           << std::hex << std::setw(2) << std::setfill('0') << ((byte_len >> 24) & 0xFF);
    body_str.replace(2 + 8, 8, len_ss.str());  // 替换长度占位符

    // 3. 计算校验和（包含长度字段）
    std::vector<uint8_t> data = hexStringToBytes(body_str);
    uint32_t checksum = calculateChecksum(data);
    std::ostringstream checksum_ss;
    checksum_ss << std::hex << std::setw(2) << std::setfill('0') << ((checksum >> 0) & 0xFF)
                << std::hex << std::setw(2) << std::setfill('0') << ((checksum >> 8) & 0xFF)
                << std::hex << std::setw(2) << std::setfill('0') << ((checksum >> 16) & 0xFF)
                << std::hex << std::setw(2) << std::setfill('0') << ((checksum >> 24) & 0xFF);
    body_str.replace(2, 8, checksum_ss.str());  // 替换校验和占位符

    // 4. 拼接完整消息
    oss << body_str << "#";  // 结束符
    return oss.str();
}

//------------------------------------------------------------------
// 解码消息
AcousticMessage ProtocolHandler::decodeMessage(const std::string& hex_data) {
    AcousticMessage msg;

    // 校验起始符和结束符
    if (hex_data.empty() || hex_data[0] != '@' || hex_data.back() != '#') {
        throw std::runtime_error("Invalid message format");
    }

    // 提取主体内容（去掉@和#）
    std::string content = hex_data.substr(1, hex_data.size() - 2);
    if (content.size() < 8 + 8 + 4 * 2 + 8) {  // 最小长度校验
        throw std::runtime_error("Message too short");
    }

    // 转换为字节数组
    std::vector<uint8_t> data = hexStringToBytes(content);

    // 解析字段
    size_t pos = 0;
    msg.type = data[pos++];

    // 读取校验和（小端序）
    msg.checksum = 0;
    for (int i = 0; i < 4; i++) {
        msg.checksum |= static_cast<uint32_t>(data[pos++]) << (i * 8);
    }

    // 读取长度（小端序）
    msg.length = 0;
    for (int i = 0; i < 4; i++) {
        msg.length |= static_cast<uint32_t>(data[pos++]) << (i * 8);
    }

    // 解析固定字段
    msg.node_id = data[pos++];
    msg.mode = data[pos++];
    msg.retransmit = data[pos++];
    msg.control_mode = data[pos++];

    // 读取发送时间（小端序）
    msg.send_time = 0;
    for (int i = 0; i < 4; i++) {
        msg.send_time |= static_cast<uint32_t>(data[pos++]) << (i * 8);
    }

    // 读取用户数据（剩余字节）
    msg.user_data.assign(data.begin() + pos, data.end());

    // 校验和验证
    std::vector<uint8_t> tmp = data;
    std::fill_n(tmp.begin() + 1, 4, 0);  // 将校验和字段置零
    uint32_t expected_checksum = calculateChecksum(tmp);
    if (msg.checksum != expected_checksum) {
        throw std::runtime_error("Checksum mismatch");
    }

    return msg;
}

//------------------------------------------------------------------
// 辅助函数：计算 CRC32 校验和
uint32_t ProtocolHandler::calculateChecksum(const std::vector<uint8_t>& data) {
    boost::crc_32_type crc;
    crc.process_bytes(data.data(), data.size());
    return crc.checksum();
}

//------------------------------------------------------------------
// 辅助函数：十六进制字符串转字节数组
std::vector<uint8_t> ProtocolHandler::hexStringToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.size(); i += 2) {
        std::string byte_str = hex.substr(i, 2);
        bytes.push_back(static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16)));
    }
    return bytes;
}

//------------------------------------------------------------------
// 辅助函数：字节数组转十六进制字符串
std::string ProtocolHandler::bytesToHexString(const std::vector<uint8_t>& bytes) {
    std::ostringstream oss;
    for (uint8_t byte : bytes) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}