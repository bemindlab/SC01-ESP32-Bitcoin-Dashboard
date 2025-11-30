#ifndef TELEGRAM_CLIENT_H
#define TELEGRAM_CLIENT_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

/**
 * TelegramClient - Wrapper class for Telegram Bot API
 *
 * Provides simplified interface for sending messages and alerts
 * via Telegram bot with message queuing to prevent spam.
 */
class TelegramClient {
public:
    /**
     * Constructor
     */
    TelegramClient();

    /**
     * Destructor - cleanup resources
     */
    ~TelegramClient();

    /**
     * Initialize the Telegram bot
     * @param token Bot token from @BotFather
     * @param chatId Target chat ID to send messages to
     * @return true if initialization successful
     */
    bool begin(const String& token, const String& chatId);

    /**
     * Check if bot is ready to send messages
     * @return true if bot is initialized and WiFi connected
     */
    bool isReady() const;

    /**
     * Send a simple text message immediately
     * @param text Message text to send
     * @return true if message sent successfully
     */
    bool sendMessage(const String& text);

    /**
     * Send a formatted alert with emoji
     * @param title Alert title
     * @param message Alert message body
     * @param emoji Emoji to prepend (e.g., "🚨", "⚠️", "✅")
     * @return true if alert sent successfully
     */
    bool sendAlert(const String& title, const String& message, const String& emoji = "🔔");

    /**
     * Process bot updates - call this in main loop
     * Checks for new messages at specified interval
     */
    void update();

    /**
     * Queue a message for sending
     * Messages are sent one per update interval to avoid spam
     * @param text Message text to queue
     * @return true if message queued successfully, false if queue full
     */
    bool queueMessage(const String& text);

    /**
     * Process queued messages - called automatically by update()
     * Sends one message per call if queue not empty
     */
    void processQueue();

private:
    // Telegram bot instance
    UniversalTelegramBot* bot;

    // Secure WiFi client for HTTPS
    WiFiClientSecure client;

    // Bot configuration
    String botToken;
    String chatId;

    // Update timing
    unsigned long lastUpdate;
    unsigned long updateInterval;

    // Message queue (circular buffer)
    static const int MAX_QUEUE_SIZE = 10;
    String messageQueue[MAX_QUEUE_SIZE];
    int queueHead;  // Next position to write
    int queueTail;  // Next position to read
    int queueSize;  // Current number of messages in queue

    // Initialization flag
    bool initialized;

    /**
     * Check if message queue is full
     * @return true if queue is full
     */
    bool isQueueFull() const;

    /**
     * Check if message queue is empty
     * @return true if queue is empty
     */
    bool isQueueEmpty() const;

    /**
     * Add message to queue (internal)
     * @param text Message to add
     * @return true if added successfully
     */
    bool enqueue(const String& text);

    /**
     * Get next message from queue (internal)
     * @param text Output parameter for message text
     * @return true if message retrieved
     */
    bool dequeue(String& text);
};

// Global instance
extern TelegramClient telegramClient;

#endif // TELEGRAM_CLIENT_H
