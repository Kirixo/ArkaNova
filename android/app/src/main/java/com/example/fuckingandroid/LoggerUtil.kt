package com.example.fuckingandroid

import java.util.logging.Logger
import java.util.logging.Level

object LoggerUtil {
    private val logger = Logger.getLogger("ArkaNovaLogger")

    fun logInfo(message: String) {
        logger.log(Level.INFO, message)
    }

    fun logWarning(message: String, throwable: Throwable? = null) {
        logger.log(Level.WARNING, message, throwable)
    }

    fun logError(message: String, throwable: Throwable? = null) {
        logger.log(Level.SEVERE, message, throwable)
    }
}