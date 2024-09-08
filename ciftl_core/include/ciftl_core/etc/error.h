#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <unordered_map>
#include <utility>

namespace ciftl
{
    typedef uint32_t ErrorCode;

    // 错误码分类
    enum class ErrorCodeBase : ErrorCode
    {
        OK = 0,
        ENCODING_ERROR = 10000,
        CRYPTER_ERROR = 20000
    };

    // 编码错误码
    enum class EncodingErrorCode : ErrorCode
    {
        HEX_ERROR = ErrorCode(ErrorCodeBase::ENCODING_ERROR) + 1 * 1000,
        BIN_ERROR = ErrorCode(ErrorCodeBase::ENCODING_ERROR) + 2 * 1000,
        BASE64_ERROR = ErrorCode(ErrorCodeBase::ENCODING_ERROR) + 3 * 1000,
    };

    enum class HexEncodingErrorCode : ErrorCode
    {
        BAD_LENGTH = ErrorCode(EncodingErrorCode::HEX_ERROR) + 1,
        BAD_CHARACTER
    };

    enum class Base64EncodingErrorCode : ErrorCode
    {
        BAD_DECODING_SOURCE = ErrorCode(EncodingErrorCode::BASE64_ERROR) + 1,
    };

    // 加密器错误码
    enum class CrypterErrorCode : ErrorCode
    {
        STREAM_GENERATOR_ERROR = ErrorCode(ErrorCodeBase::CRYPTER_ERROR) + 1 * 1000,
        STREAM_CRYPTER_ERROR = ErrorCode(ErrorCodeBase::CRYPTER_ERROR) + 2 * 1000,
        STRING_CRYPTER_ERROR = ErrorCode(ErrorCodeBase::CRYPTER_ERROR) + 3 * 1000,
        // CHACHA20_ERROR = ErrorCode(ErrorCodeBase::CRYPTER_ERROR) + 4 * 1000,
    };

    enum class StreamGeneratorErrorCode : ErrorCode
    {
        FAILED_WHEN_CRYPTING = ErrorCode(CrypterErrorCode::STREAM_GENERATOR_ERROR) + 1, // 执行密码操作时失败
        FAILED_WHEN_FINALIZING_CRYPTION,                                                // 完成密码操作时失败
        FAILED_WHEN_FLUSHING_BUFFER,                                                    // 刷新缓冲区时失败
    };

    enum class StreamCrypterErrorCode : ErrorCode
    {
        INVALID_CIPHER_STREAM_GENERATOR = ErrorCode(CrypterErrorCode::STREAM_CRYPTER_ERROR) + 1, // 创建密码流生成器失败
    };

    enum class StringCrypterErrorCode : ErrorCode
    {
        CIPHERTEXT_IS_TOO_SHORT = ErrorCode(CrypterErrorCode::STRING_CRYPTER_ERROR) + 1, // 密文文本太短
        WRONG_PASSWORD,                                                                  // 密码错误
    };

    class Error
    {
        // 错误码
        ErrorCode m_error_code = ErrorCode(ErrorCodeBase::OK);
        // 错误信息
        std::string m_error_message;
        // 补充信息
        std::optional<std::string> m_optional_message;

    public:
        Error() = default;

        template <class ERROR_CODE>
        explicit Error(ERROR_CODE ec) : m_error_code(static_cast<ErrorCode>(ec))
        {
        }

        template <class ERROR_CODE>
        explicit Error(ERROR_CODE ec, const std::string &message, std::optional<std::string> optional_message = std::nullopt) : m_error_code(static_cast<ErrorCode>(ec)),
                                                                                                                                m_error_message(message),
                                                                                                                                m_optional_message(optional_message)
        {
        }

        template <class ERROR_CODE>
        explicit Error(ERROR_CODE ec, std::string &&message, std::optional<std::string> optional_message = std::nullopt) : m_error_code(static_cast<ErrorCode>(ec)),
                                                                                                                           m_error_message(std::move(message)),
                                                                                                                           m_optional_message(optional_message)
        {
        }

        Error(const Error &error) = default;

        Error(Error &&error) = default;

    public:
        inline ErrorCode get_error_code() const noexcept
        {
            return m_error_code;
        }

        inline const std::string &get_error_message() const noexcept
        {
            return m_error_message;
        }
    };

    template <class OK_TYPE, class ERROR_TYPE>
    class CoreResult
    {
        // using OK_PTR_TYPE = OK_TYPE*;
        // using OK_CONST_PTR_TYPE = const OK_PTR_TYPE;
        // using OK_REF_TYPE = OK_TYPE&;
        // using OK_CONST_REF_TYPE = const OK_REF_TYPE;

        // OK_TYPE和ERROR_TYPE不能相同
        static_assert(!std::is_same_v<OK_TYPE, ERROR_TYPE>, "OK type cannot be same with Error type!");
        // static_assert(std::is_move_constructible<OK_TYPE>::value, "OK type should be movely constructed!");

        // OK类型的对象
        OK_TYPE m_ok_val;
        // Error类型的对象
        ERROR_TYPE m_err_val;
        bool m_is_ok = true;

    public:
        explicit CoreResult(const OK_TYPE &ok_val) : m_ok_val(ok_val),
                                                     m_is_ok(true)
        {
        }

        explicit CoreResult(OK_TYPE &&ok_val) : m_ok_val(std::move(ok_val)),
                                                m_is_ok(true)
        {
        }

        explicit CoreResult(const ERROR_TYPE &err_val) : m_err_val(err_val),
                                                         m_is_ok(false)
        {
        }

        explicit CoreResult(ERROR_TYPE &&err_val) : m_err_val(std::move(err_val)),
                                                    m_is_ok(false)
        {
        }

        CoreResult(const CoreResult &res) noexcept
            : m_ok_val(res.m_ok_val),
              m_err_val(res.m_err_val),
              m_is_ok(res.m_is_ok)
        {
        }

        CoreResult(CoreResult &&res) noexcept
            : m_ok_val(std::move(res.m_ok_val)),
              m_err_val(std::move(res.m_err_val)),
              m_is_ok(res.m_is_ok)
        {
        }

        bool operator==(CoreResult) = delete;

        CoreResult &operator=(const CoreResult &res) noexcept
        {
            m_ok_val = res.m_ok_val;
            m_err_val = res.m_err_val;
            m_is_ok = res.m_is_ok;
            return *this;
        }

        CoreResult &operator=(CoreResult &&res) noexcept
        {
            m_ok_val = std::move(res.m_ok_val);
            m_err_val = std::move(res.m_err_val);
            m_is_ok = res.m_is_ok;
            return *this;
        }

        ~CoreResult() = default;

    public:
        [[nodiscard]] bool is_ok() const noexcept
        {
            return m_is_ok;
        }

        [[nodiscard]] bool is_err() const noexcept
        {
            return !is_ok();
        }

        [[nodiscard]] std::optional<OK_TYPE> get_ok_val() const noexcept
        {
            return is_ok() ? std::make_optional<OK_TYPE>(m_ok_val) : std::nullopt;
        }

        [[nodiscard]] std::optional<ERROR_TYPE> get_err_val() const noexcept
        {
            return is_err() ? std::make_optional<ERROR_TYPE>(m_err_val) : std::nullopt;
        }
    };

    // 重定义Result
    template <class OK_TYPE>
    using Result = CoreResult<OK_TYPE, Error>;

    template <class OK_TYPE>
    Result<OK_TYPE> make_error(const Error &error)
    {
        return Result<OK_TYPE>(error);
    }

    template <class OK_TYPE>
    Result<OK_TYPE> make_error(Error &&error)
    {
        return Result<OK_TYPE>(std::move(error));
    }

    template <class OK_TYPE, class ERROR_CODE>
    Result<OK_TYPE> make_error(ERROR_CODE ec, const std::string &message)
    {
        return Result<OK_TYPE>(std::move(Error(ec, message)));
    }

    template <class OK_TYPE, class ERROR_CODE>
    Result<OK_TYPE> make_error(ERROR_CODE ec, std::string &&message)
    {
        return Result<OK_TYPE>(std::move(Error(ec, std::move(message))));
    }

    template <class OK_TYPE>
    Result<OK_TYPE> make_ok(const OK_TYPE &ok_val)
    {
        return Result<OK_TYPE>(ok_val);
    }

    template <class OK_TYPE>
    Result<OK_TYPE> make_ok(OK_TYPE &&ok_val)
    {
        return Result<OK_TYPE>(std::move(ok_val));
    }
}
