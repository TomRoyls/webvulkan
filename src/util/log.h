/**
 * @file log.h
 * @brief Logging utility for webvulkan
 * 
 * Provides configurable logging with levels and optional output redirection.
 */

#ifndef WGVK_LOG_H
#define WGVK_LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Log levels for webvulkan messages
 */
typedef enum {
    WGVK_LOG_ERROR = 0,   /**< Errors that prevent normal operation */
    WGVK_LOG_WARN  = 1,   /**< Warnings about potential issues */
    WGVK_LOG_INFO  = 2,   /**< General information messages */
    WGVK_LOG_DEBUG = 3,   /**< Detailed debugging information */
    WGVK_LOG_TRACE = 4,   /**< Very verbose trace information */
} WgvkLogLevel;

/**
 * Log category for filtering messages
 */
typedef enum {
    WGVK_LOG_CAT_GENERAL   = 0,  /**< General messages */
    WGVK_LOG_CAT_CORE      = 1,  /**< Core (instance, device) */
    WGVK_LOG_CAT_MEMORY    = 2,  /**< Memory operations */
    WGVK_LOG_CAT_SHADER    = 3,  /**< Shader compilation */
    WGVK_LOG_CAT_PIPELINE  = 4,  /**< Pipeline creation */
    WGVK_LOG_CAT_COMMAND   = 5,  /**< Command buffer operations */
    WGVK_LOG_CAT_SYNC      = 6,  /**< Synchronization */
    WGVK_LOG_CAT_MAX
} WgvkLogCategory;

/**
 * Custom log callback function type
 * @param level Log level of the message
 * @param category Log category
 * @param message The formatted message
 * @param user_data User-provided context
 */
typedef void (*WgvkLogCallback)(WgvkLogLevel level, WgvkLogCategory category,
                                const char* message, void* user_data);

/**
 * Initialize the logging system
 * Should be called once at application start.
 */
void wgvk_log_init(void);

/**
 * Set the minimum log level
 * Messages below this level will be suppressed.
 * @param level Minimum level to display
 */
void wgvk_log_set_level(WgvkLogLevel level);

/**
 * Get the current log level
 * @return Current minimum log level
 */
WgvkLogLevel wgvk_log_get_level(void);

/**
 * Enable or disable a log category
 * @param category Category to configure
 * @param enabled Non-zero to enable, zero to disable
 */
void wgvk_log_set_category_enabled(WgvkLogCategory category, int enabled);

/**
 * Check if a category is enabled
 * @param category Category to check
 * @return Non-zero if enabled, zero if disabled
 */
int wgvk_log_is_category_enabled(WgvkLogCategory category);

/**
 * Set a custom log callback
 * If set, all log messages will be sent to this callback instead of stderr.
 * @param callback Callback function (NULL to use default)
 * @param user_data User context passed to callback
 */
void wgvk_log_set_callback(WgvkLogCallback callback, void* user_data);

/**
 * Core logging function (use macros instead)
 * @param level Log level
 * @param category Log category
 * @param file Source file name
 * @param line Source line number
 * @param func Function name
 * @param fmt Printf-style format string
 * @param ... Format arguments
 */
void wgvk_log_message(WgvkLogLevel level, WgvkLogCategory category,
                      const char* file, int line, const char* func,
                      const char* fmt, ...);

/**
 * Core logging function with va_list (use macros instead)
 */
void wgvk_log_message_v(WgvkLogLevel level, WgvkLogCategory category,
                        const char* file, int line, const char* func,
                        const char* fmt, va_list args);

/* Convenience macros for logging */

#define WGVK_ERROR(cat, fmt, ...) \
    wgvk_log_message(WGVK_LOG_ERROR, cat, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define WGVK_WARN(cat, fmt, ...) \
    wgvk_log_message(WGVK_LOG_WARN, cat, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define WGVK_INFO(cat, fmt, ...) \
    wgvk_log_message(WGVK_LOG_INFO, cat, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define WGVK_DEBUG(cat, fmt, ...) \
    wgvk_log_message(WGVK_LOG_DEBUG, cat, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define WGVK_TRACE(cat, fmt, ...) \
    wgvk_log_message(WGVK_LOG_TRACE, cat, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

/* Category-specific convenience macros */

#define WGVK_LOG_CORE(fmt, ...)    WGVK_DEBUG(WGVK_LOG_CAT_CORE, fmt, ##__VA_ARGS__)
#define WGVK_LOG_MEMORY(fmt, ...)  WGVK_DEBUG(WGVK_LOG_CAT_MEMORY, fmt, ##__VA_ARGS__)
#define WGVK_LOG_SHADER(fmt, ...)  WGVK_DEBUG(WGVK_LOG_CAT_SHADER, fmt, ##__VA_ARGS__)
#define WGVK_LOG_PIPELINE(fmt, ...) WGVK_DEBUG(WGVK_LOG_CAT_PIPELINE, fmt, ##__VA_ARGS__)
#define WGVK_LOG_COMMAND(fmt, ...) WGVK_DEBUG(WGVK_LOG_CAT_COMMAND, fmt, ##__VA_ARGS__)
#define WGVK_LOG_SYNC(fmt, ...)    WGVK_DEBUG(WGVK_LOG_CAT_SYNC, fmt, ##__VA_ARGS__)

/* String conversion utilities */

/**
 * Convert log level to string
 * @param level Log level
 * @return Static string representation
 */
const char* wgvk_log_level_string(WgvkLogLevel level);

/**
 * Convert log category to string
 * @param category Log category
 * @return Static string representation
 */
const char* wgvk_log_category_string(WgvkLogCategory category);

#ifdef __cplusplus
}
#endif

#endif /* WGVK_LOG_H */
