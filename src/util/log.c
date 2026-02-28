/**
 * @file log.c
 * @brief Logging utility implementation
 */

#include "log.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static struct {
	WgvkLogLevel level;
	int category_enabled[WGVK_LOG_CAT_MAX];
	WgvkLogCallback callback;
	void *callback_user_data;
	int initialized;
} g_log = {.level = WGVK_LOG_INFO, .callback = NULL, .callback_user_data = NULL, .initialized = 0};

static const char *level_strings[] = {"ERROR", "WARN", "INFO", "DEBUG", "TRACE"};

static const char *category_strings[] = {"general",  "core",    "memory", "shader",
                                         "pipeline", "command", "sync"};

void wgvk_log_init(void) {
	if (g_log.initialized) {
		return;
	}

	g_log.level = WGVK_LOG_INFO;
	g_log.callback = NULL;
	g_log.callback_user_data = NULL;

	for (int i = 0; i < WGVK_LOG_CAT_MAX; i++) {
		g_log.category_enabled[i] = 1;
	}

	g_log.initialized = 1;
}

void wgvk_log_set_level(WgvkLogLevel level) {
	if (!g_log.initialized) {
		wgvk_log_init();
	}
	g_log.level = level;
}

WgvkLogLevel wgvk_log_get_level(void) {
	return g_log.level;
}

void wgvk_log_set_category_enabled(WgvkLogCategory category, int enabled) {
	if (!g_log.initialized) {
		wgvk_log_init();
	}
	if (category >= 0 && category < WGVK_LOG_CAT_MAX) {
		g_log.category_enabled[category] = enabled ? 1 : 0;
	}
}

int wgvk_log_is_category_enabled(WgvkLogCategory category) {
	if (category >= 0 && category < WGVK_LOG_CAT_MAX) {
		return g_log.category_enabled[category];
	}
	return 0;
}

void wgvk_log_set_callback(WgvkLogCallback callback, void *user_data) {
	if (!g_log.initialized) {
		wgvk_log_init();
	}
	g_log.callback = callback;
	g_log.callback_user_data = user_data;
}

void wgvk_log_message_v(WgvkLogLevel level, WgvkLogCategory category, const char *file, int line,
                        const char *func, const char *fmt, va_list args) {
	if (!g_log.initialized) {
		wgvk_log_init();
	}

	if (level > g_log.level) {
		return;
	}

	if (category >= 0 && category < WGVK_LOG_CAT_MAX) {
		if (!g_log.category_enabled[category]) {
			return;
		}
	}

	char message[4096];
	int written = vsnprintf(message, sizeof(message), fmt, args);
	if (written < 0 || (size_t)written >= sizeof(message)) {
		message[sizeof(message) - 1] = '\0';
	}

	if (g_log.callback) {
		g_log.callback(level, category, message, g_log.callback_user_data);
		return;
	}

	time_t now = time(NULL);
	struct tm *tm_info = localtime(&now);
	char time_buf[32];
	strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);

	const char *filename = strrchr(file, '/');
	filename = filename ? filename + 1 : file;

	fprintf(stderr, "[%s] [%-5s] [%-8s] %s (%s:%d in %s)\n", time_buf, wgvk_log_level_string(level),
	        wgvk_log_category_string(category), message, filename, line, func);
	fflush(stderr);
}

void wgvk_log_message(WgvkLogLevel level, WgvkLogCategory category, const char *file, int line,
                      const char *func, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	wgvk_log_message_v(level, category, file, line, func, fmt, args);
	va_end(args);
}

const char *wgvk_log_level_string(WgvkLogLevel level) {
	if (level >= WGVK_LOG_ERROR && level <= WGVK_LOG_TRACE) {
		return level_strings[level];
	}
	return "UNKNOWN";
}

const char *wgvk_log_category_string(WgvkLogCategory category) {
	if (category >= 0 && category < WGVK_LOG_CAT_MAX) {
		return category_strings[category];
	}
	return "unknown";
}
