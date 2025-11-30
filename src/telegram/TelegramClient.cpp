#include "TelegramClient.h"
#include <WiFi.h>

// Global instance
TelegramClient telegramClient;

// Constructor
TelegramClient::TelegramClient()
    : bot(nullptr),
      botToken(""),
      chatId(""),
      lastUpdate(0),
      updateInterval(1000), // 1 second
      queueHead(0),
      queueTail(0),
      queueSize(0),
      initialized(false) {
    // Initialize secure client (no certificate validation for simplicity)
    client.setInsecure();
}

// Destructor
TelegramClient::~TelegramClient() {
    if (bot != nullptr) {
        delete bot;
        bot = nullptr;
    }
}

// Initialize the bot
bool TelegramClient::begin(const String& token, const String& chatId) {
    if (token.length() == 0 || chatId.length() == 0) {
        Serial.println("[Telegram] Error: Invalid token or chat ID");
        return false;
    }

    this->botToken = token;
    this->chatId = chatId;

    // Create bot instance
    if (bot != nullptr) {
        delete bot;
    }
    bot = new UniversalTelegramBot(botToken, client);

    if (bot == nullptr) {
        Serial.println("[Telegram] Error: Failed to create bot instance");
        return false;
    }

    initialized = true;
    Serial.println("[Telegram] Bot initialized successfully");
    Serial.printf("[Telegram] Chat ID: %s\n", chatId.c_str());

    return true;
}

// Check if bot is ready
bool TelegramClient::isReady() const {
    return initialized && WiFi.status() == WL_CONNECTED;
}

// Send a simple text message
bool TelegramClient::sendMessage(const String& text) {
    if (!isReady()) {
        Serial.println("[Telegram] Error: Bot not ready or WiFi disconnected");
        return false;
    }

    if (text.length() == 0) {
        Serial.println("[Telegram] Error: Empty message");
        return false;
    }

    Serial.printf("[Telegram] Sending message: %s\n", text.c_str());

    bool sent = bot->sendMessage(chatId, text, "");

    if (sent) {
        Serial.println("[Telegram] Message sent successfully");
    } else {
        Serial.println("[Telegram] Error: Failed to send message");
    }

    return sent;
}

// Send a formatted alert
bool TelegramClient::sendAlert(const String& title, const String& message, const String& emoji) {
    if (!isReady()) {
        Serial.println("[Telegram] Error: Bot not ready for alert");
        return false;
    }

    // Format alert message with emoji and markdown
    String formattedMessage = emoji + " *" + title + "*\n\n" + message;

    Serial.printf("[Telegram] Sending alert: %s\n", title.c_str());

    bool sent = bot->sendMessage(chatId, formattedMessage, "Markdown");

    if (sent) {
        Serial.println("[Telegram] Alert sent successfully");
    } else {
        Serial.println("[Telegram] Error: Failed to send alert");
    }

    return sent;
}

// Process bot updates
void TelegramClient::update() {
    if (!isReady()) {
        return;
    }

    unsigned long currentMillis = millis();

    // Check if update interval has passed
    if (currentMillis - lastUpdate >= updateInterval) {
        lastUpdate = currentMillis;

        // Process message queue
        processQueue();

        // Check for new messages (optional - can be used for bot commands)
        // Uncomment if you want to receive messages:
        // int numNewMessages = bot->getUpdates(bot->last_message_received + 1);
        // if (numNewMessages > 0) {
        //     Serial.printf("[Telegram] Received %d new messages\n", numNewMessages);
        //     // Handle messages here
        // }
    }
}

// Queue a message
bool TelegramClient::queueMessage(const String& text) {
    if (text.length() == 0) {
        Serial.println("[Telegram] Error: Cannot queue empty message");
        return false;
    }

    if (isQueueFull()) {
        Serial.println("[Telegram] Error: Message queue full");
        return false;
    }

    return enqueue(text);
}

// Process queued messages
void TelegramClient::processQueue() {
    if (isQueueEmpty()) {
        return;
    }

    String message;
    if (dequeue(message)) {
        Serial.printf("[Telegram] Processing queued message (%d remaining)\n", queueSize);
        sendMessage(message);
    }
}

// Check if queue is full
bool TelegramClient::isQueueFull() const {
    return queueSize >= MAX_QUEUE_SIZE;
}

// Check if queue is empty
bool TelegramClient::isQueueEmpty() const {
    return queueSize == 0;
}

// Add message to queue
bool TelegramClient::enqueue(const String& text) {
    if (isQueueFull()) {
        return false;
    }

    messageQueue[queueHead] = text;
    queueHead = (queueHead + 1) % MAX_QUEUE_SIZE;
    queueSize++;

    Serial.printf("[Telegram] Message queued (%d in queue)\n", queueSize);
    return true;
}

// Get next message from queue
bool TelegramClient::dequeue(String& text) {
    if (isQueueEmpty()) {
        return false;
    }

    text = messageQueue[queueTail];
    queueTail = (queueTail + 1) % MAX_QUEUE_SIZE;
    queueSize--;

    return true;
}
