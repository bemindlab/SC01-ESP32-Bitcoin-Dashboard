#ifndef BTC_DATA_H
#define BTC_DATA_H

#include <Arduino.h>

struct BTCData {
    float priceUSD = 0;
    float priceEUR = 0;
    unsigned long blockHeight = 0;
    char blockHash[65] = "";
    int blockTxCount = 0;
    uint32_t blockTime = 0;
    unsigned long mempoolCount = 0;
    float mempoolSize = 0;
    int feeFast = 0;
    int feeMedium = 0;
    int feeSlow = 0;

    // AI-powered analysis (Gemini)
    char dcaRecommendation[32] = "Wait";  // BUY/SELL/WAIT
    char tradingSignal[32] = "HOLD";      // BUY/SELL/HOLD
    char signalTimeframe[16] = "15m-1h";  // Timeframe for signal
};

#endif
