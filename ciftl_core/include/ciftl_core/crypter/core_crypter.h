#pragma once
#include <cstring>
#include <memory>
#include <unordered_map>
#include <functional>
#include <random>

#include <crc32c/crc32c.h>

#include <ciftl_core/etc/etc.h>
#include <ciftl_core/hash/hash.h>
#include <ciftl_core/encoding/encoding.h>
#include <openssl/asn1.h>

namespace ciftl
{
    enum class CipherAlgorithm
    {
        ChaCha20,
        AES,
        SM4
    };

    // 加密密码流生成器的不同模式
    // 该选项会影响到StreamGenerator中的预生成流长度，
    // 加密短文本时使用Short可以避免生成过长的预生成流
    // 加密长文件时使用Large可以减少预生成流的生成次数
    enum class StreamGeneratorMode : size_t
    {
        Short = 1,
        Medium = 32,
        Large = 1024
    };

    // 随机生成IV
    inline void rand_iv(byte *data, size_t len) noexcept
    {
        // 使用随机数引擎和分布来生成随机字节
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for (size_t i = 0; i < len; ++i)
        {
            data[i] = (byte)(dis(gen) & 0xFF);
        }
        return;
    }

    // 根据iv因子计算出iv
    template <class Factor>
    inline void generate_iv_from_factor(Factor factor, byte *data, size_t len) noexcept
    {
        Sha256Hasher hasher;
        hasher.update(auto_cast<ByteVector>(factor));
        ByteVector buffer = hasher.finalize();
        size_t cnt = 0;
        for (; cnt < len;)
        {
            size_t once_gen = std::min(len - cnt, buffer.size());
            memcpy(data + cnt, buffer.data(), once_gen);
            cnt += once_gen;
            Sha256Hasher hasher;
            hasher.update(buffer);
            buffer = hasher.finalize();
        }
    }

    // 根据密码生成密钥
    inline void generate_key_from_password(const char *password, byte *data, size_t len) noexcept
    {
        // 生成加密所需的密钥
        Sha256Hasher hasher;
        hasher.update(password);
        ByteVector buffer = hasher.finalize();
        size_t cnt = 0;
        for (; cnt < len;)
        {
            size_t once_gen = std::min(len - cnt, buffer.size());
            memcpy(data + cnt, buffer.data(), once_gen);
            cnt += once_gen;
            Sha256Hasher hasher;
            hasher.update(buffer);
            buffer = hasher.finalize();
        }
    }

    // 密码流生成器的接口
    class StreamGeneratorInterface
    {
    public:
        // 生成密码流
        virtual Result<size_t> generate(byte *data, size_t len) = 0;
        virtual Result<size_t> generate(ByteVector &data) = 0;
        // 刷新缓冲区内容
        virtual Result<size_t> flush() = 0;
    };

    // 加密密码流生成器
    // 密码流生成器有两个关键的参数，一个是IV长度，一个是密钥长度
    // 所有密码流生成器在继承是都需要指定这两个参数
    // 同时，为了适用于分组密码，BLOCK_LENGTH也作为可选参数传入，默认为0表示为流密码
    // 为了避免在将分组密码数组转换成密码流的过程时出现缓冲区大小无法被分组块大小整除的情况（也就是分组在填充到缓冲区时会有剩余）
    // 这里需要规定缓冲区大小一定是分组长度的倍数
    template <size_t M_IV_LENGTH, size_t M_KEY_LENGTH, size_t M_BLOCK_LENGTH = 0>
    class StreamGenerator : public StreamGeneratorInterface
    {
    public:
        static constexpr size_t IV_LENGTH = M_IV_LENGTH;
        static constexpr size_t KEY_LENGTH = M_KEY_LENGTH;
        static constexpr size_t BLOCK_LENGTH = M_BLOCK_LENGTH;
        typedef ByteArray<M_IV_LENGTH> IVByteArray;
        typedef ByteArray<M_KEY_LENGTH> KeyByteArray;

    public:
        // 获取不同模式下的缓存区的分组块大小
        static constexpr size_t stream_temp_block_count(StreamGeneratorMode m)
        {
            return static_cast<size_t>(m) * 64;
        }

        // 获取不同模式下的缓存区大小
        static constexpr size_t stream_temp_buffer_size(StreamGeneratorMode m)
        {
            if(BLOCK_LENGTH)
            {
                return stream_temp_block_count(m) * BLOCK_LENGTH;
            }
            return static_cast<size_t>(m) * 1024;
        }

        static constexpr size_t iv_length()
        {
            return M_IV_LENGTH;
        }

        static constexpr size_t key_length()
        {
            return M_KEY_LENGTH;
        }

        // 随机生成IV
        static IVByteArray rand_iv()
        {
            IVByteArray iv_bytes;
            ::ciftl::rand_iv(iv_bytes.data(), iv_bytes.size());
            return iv_bytes;
        }

        // 根据密码生成密钥
        template <class Factor>
        static IVByteArray generate_iv_from_factor(Factor factor) noexcept
        {
            IVByteArray iv_bytes;
            ::ciftl::generate_iv_from_factor(factor, iv_bytes.data(), iv_bytes.size());
            return iv_bytes;
        }

        // 根据密码生成密钥
        static KeyByteArray generate_key_from_password(const std::string &password)
        {
            // 生成加密所需的密钥
            KeyByteArray key;
            ::ciftl::generate_key_from_password(password.c_str(), key.data(), key.size());
            return key;
        }

    public:
        StreamGenerator(const IVByteArray &iv,
                        const KeyByteArray &key,
                        StreamGeneratorMode mode = StreamGeneratorMode::Medium,
                        std::function<size_t(StreamGeneratorMode)> assign_buffer_size = stream_temp_buffer_size)
            : m_mode(mode),
              m_max_buffer_size(assign_buffer_size(mode)),
              m_current_index(0L),
              m_current_buffer(std::make_unique<byte[]>(m_max_buffer_size)),
              m_plaintext_buffer(std::make_unique<byte[]>(m_max_buffer_size)),
              m_iv(iv),
              m_key(key),
              m_is_flush_init(false)
        {
            memset(m_plaintext_buffer.get(), 0x00, m_max_buffer_size);
        }

        ~StreamGenerator() = default;

    public:
        // 生成加密流
        Result<size_t> generate(byte *data, size_t len) override
        {
            // 初始化刷新
            if (!m_is_flush_init)
            {
                if (auto res = flush(); res.is_err())
                {
                    return res;
                }
                m_is_flush_init = true;
            }
            size_t index = 0L;
            size_t once_gen = m_max_buffer_size - m_current_index;
            // 如果要产生新的buffer，则不断的循环这一步骤
            for (; index + once_gen < len;)
            {
                memcpy(data + index, m_current_buffer.get() + m_current_index, once_gen);
                index += once_gen;
                m_current_index += once_gen;
                if (auto res = flush(); res.is_err())
                {
                    return res;
                }
                once_gen = m_max_buffer_size - m_current_index;
            }
            size_t last_gen = len - index;
            memcpy(data + index, m_current_buffer.get() + m_current_index, last_gen);
            index += last_gen;
            m_current_index += last_gen;
            return make_ok<size_t>(len);
        }

        // 生成密码流
        Result<size_t> generate(ByteVector &data) override
        {
            return generate(data.data(), data.size());
        }

        // 刷新缓冲区内容
        Result<size_t> flush() override
        {
            return make_ok<size_t>(m_max_buffer_size);
        }

    protected:
        // 缓冲区最大容量
        const size_t m_max_buffer_size;
        // 流生成模式
        StreamGeneratorMode m_mode;
        size_t m_current_index;
        // 当前的密码流缓冲区
        std::unique_ptr<byte[]> m_current_buffer;
        // 用于作为加密原文的明文缓冲区
        std::unique_ptr<byte[]> m_plaintext_buffer;
        // 密码流生成器使用的IV和Key
        IVByteArray m_iv;
        KeyByteArray m_key;
        bool m_is_flush_init;
    };

    // OpenSSL加密密码流生成器
    template <size_t M_IV_LENGTH, size_t M_KEY_LENGTH, size_t M_BLOCK_LENGTH = 0>
    class StreamGeneratorForOpenSSL : public StreamGenerator<M_IV_LENGTH, M_KEY_LENGTH, M_BLOCK_LENGTH>
    {
    public:
        static constexpr size_t IV_LENGTH = M_IV_LENGTH;
        static constexpr size_t KEY_LENGTH = M_KEY_LENGTH;
        static constexpr size_t BLOCK_LENGTH = M_BLOCK_LENGTH;
        typedef typename StreamGenerator<M_IV_LENGTH, M_KEY_LENGTH, M_BLOCK_LENGTH>::IVByteArray IVByteArray;
        typedef typename StreamGenerator<M_IV_LENGTH, M_KEY_LENGTH, M_BLOCK_LENGTH>::KeyByteArray KeyByteArray;

    public:
        StreamGeneratorForOpenSSL(const IVByteArray &iv,
                        const KeyByteArray &key,
                        StreamGeneratorMode mode = StreamGeneratorMode::Medium)
            : StreamGenerator<M_IV_LENGTH, M_KEY_LENGTH, M_BLOCK_LENGTH>(iv, key, mode),
            m_ctx(EVP_CIPHER_CTX_new())
        {
            if (!m_ctx)
            {
                throw std::bad_alloc();
            }
        }

        ~StreamGeneratorForOpenSSL() {
            if(m_ctx)
            {
                EVP_CIPHER_CTX_free(m_ctx);
            }
            m_ctx = nullptr;
        }

    public:
       // 刷新缓冲区内容
        Result<size_t> flush() override
        {
            if (this->m_current_index != this->m_max_buffer_size && this->m_is_flush_init)
            {
                return make_error<size_t>(StreamGeneratorErrorCode::FAILED_WHEN_FLUSHING_BUFFER, "刷新缓冲区时失败");
            }
            this->m_current_index = 0;
            auto temp_buffer = std::make_unique<byte[]>(this->m_max_buffer_size);
            // 执行加密，这里是对m_plaintext_buffer进行加密，并将结果拷贝到temp_buffer中
            int out_len;
            if (!EVP_EncryptUpdate(m_ctx, temp_buffer.get(), &out_len, this->m_plaintext_buffer.get(), (int)this->m_max_buffer_size))
            {
                return make_error<size_t>(StreamGeneratorErrorCode::FAILED_WHEN_CRYPTING, "执行密码操作时失败");
            }
            // 传入长度应该和输出长度一致
            if (out_len != this->m_max_buffer_size)
            {
                return make_error<size_t>(StreamGeneratorErrorCode::FAILED_WHEN_CRYPTING, "执行密码操作时失败");
            }
            memcpy(this->m_current_buffer.get(), temp_buffer.get(), this->m_max_buffer_size);
            return make_ok<size_t>(this->m_max_buffer_size);
        }

    protected:
        // OpenSSL上下文
        EVP_CIPHER_CTX *m_ctx;
    };

    // 密码流加密器
    // 密码流加密器会使用密码流生成器来生成一段密文，并且用这一段密文对明文进行加密或者解密
    // 此外，密码流加密器还会计算处理前后的crc32值用于校验密文是否被篡改
    class StreamCrypter
    {
    public:
        StreamCrypter(std::shared_ptr<StreamGeneratorInterface> generator);
        ~StreamCrypter() = default;

        Result<size_t> crypt(byte *data, size_t len);
        Result<size_t> crypt(ByteVector &data);

        uint32_t original_crc32();
        uint32_t crypted_crc32();

    private:
        // 流生成器
        std::shared_ptr<StreamGeneratorInterface> m_stream_generator;
        // 原始数据的crc32
        uint32_t m_original_crc32;
        // 处理后的数据的crc32
        uint32_t m_crypted_crc32;
    };

    // 字符串加密器接口
    class StringCrypterInterface
    {
    public:
        virtual Result<std::string> encrypt(const std::string &data,
                                            const std::string &password) = 0;

        virtual Result<std::string> decrypt(const std::string &data,
                                            const std::string &password) = 0;
    };

    // 字符串加密器
    // 字符串加密器（StringCrypter）与StreamCrypter不同，
    // StreamCrypter是StreamGenerator的包装器，会被多次的调用来加密一段文本
    // StringCrypter基于StreamCrypter和StreamGenerator实现了对一段文本的加密，每次调用都是独立的
    // StringCrypter密文格式
    // |-----------IV（长度取决于加密算法）-----|----CRC32（4字节，会在最后被加密）-----|--------密文（使用StreamGenerator进行加密）--------|
    // 暴露的信息只有IV，解密时通过IV和密码生成的密钥来初始化StreamGenerator
    template <class STREAM_GENERATOR>
    class StringCrypter : public StringCrypterInterface
    {
    public:
        static constexpr size_t IV_LENGTH = STREAM_GENERATOR::IV_LENGTH;
        static constexpr size_t KEY_LENGTH = STREAM_GENERATOR::KEY_LENGTH;
        typedef typename STREAM_GENERATOR::IVByteArray IVByteArray;
        typedef typename STREAM_GENERATOR::KeyByteArray KeyByteArray;

    protected:
        std::shared_ptr<Encoding> m_encoding;

    public:
        // 字符串加密器结果默认使用base64进行编码
        StringCrypter(std::shared_ptr<Encoding> encoding = default_base64_encoding())
            : m_encoding(encoding)
        {
            if (!encoding)
            {
                throw std::bad_alloc();
            }
        }

        ~StringCrypter() = default;

    public:
        Result<std::string> encrypt(const std::string &text,
                                    const std::string &password) override
        {
            // 随机生成IV
            IVByteArray iv = STREAM_GENERATOR::rand_iv();
            // 生成Key
            KeyByteArray key = STREAM_GENERATOR::generate_key_from_password(password);
            // 创建密码流对象
            StreamCrypter sc = StreamCrypter(std::make_shared<STREAM_GENERATOR>(iv, key, StreamGeneratorMode::Short));
            ByteVector raw_data = auto_cast<ByteVector>(text);
            if (auto res = sc.crypt(raw_data.data(), raw_data.size()); res.is_err())
            {
                return make_error<std::string>(std::move(res.get_err_val().value()));
            }
            // 用剩余的密码流加密crc32校验码
            Crc32ByteArray original_crc32 = auto_cast<Crc32ByteArray>(sc.original_crc32());
            if (auto res = sc.crypt(original_crc32.data(), original_crc32.size()); res.is_err())
            {
                return make_error<std::string>(std::move(res.get_err_val().value()));
            }
            ByteVector combined_res = combine(iv, original_crc32, raw_data);
            // 编码
            std::string res = m_encoding->encode(combined_res);
            return make_ok<std::string>(res);
        }

        Result<std::string> decrypt(const std::string &text,
                                    const std::string &password) override
        {
            ByteVector raw_data;
            // 解码
            if (auto res = m_encoding->decode(text); res.is_err())
            {
                return make_error<std::string>(std::move(res.get_err_val().value()));
            }
            else
            {
                raw_data = res.get_ok_val().value();
            }
            if (raw_data.size() < IV_LENGTH + sizeof(Crc32Value))
            {
                return make_error<std::string>(StringCrypterErrorCode::CIPHERTEXT_IS_TOO_SHORT, "密文长度太短");
            }
            // 使用divide函数将源数据流分配到目标数组中
            IVByteArray iv;
            Crc32ByteArray crc32_array;
            ByteVector data_body(raw_data.size() - iv.size() - crc32_array.size());
            divide(raw_data, 0, iv, crc32_array, data_body);
            // 进行解密
            KeyByteArray key = STREAM_GENERATOR::generate_key_from_password(password);
            StreamCrypter sc = StreamCrypter(std::make_shared<STREAM_GENERATOR>(iv, key, StreamGeneratorMode::Short));
            // 根据加密逻辑，先解密数据部分，后解密crc32校验部分
            if (auto res = sc.crypt(data_body.data(), data_body.size()); res.is_err())
            {
                return make_error<std::string>(std::move(res.get_err_val().value()));
            }
            // 注意，解密crc32时会改变sc中的crc32值，所以要提前把值取出来
            Crc32Value crypted_crc32 = sc.crypted_crc32();
            // 解密原来的crc32值
            if (auto res = sc.crypt(crc32_array.data(), crc32_array.size()); res.is_err())
            {
                return make_error<std::string>(std::move(res.get_err_val().value()));
            }
            // 比较解密后得到的结果的校验码和原校验码
            if (!compare(auto_cast<Crc32ByteArray>(crypted_crc32), crc32_array))
            {
                return make_error<std::string>(StringCrypterErrorCode::WRONG_PASSWORD, "密码错误");
            }
            std::string res = auto_cast<std::string>(data_body);
            return make_ok<std::string>(res);
        }
    };
}
