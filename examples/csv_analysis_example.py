#!/usr/bin/env python3
"""
Bitcoin Dashboard - CSV Data Analysis Example

This script demonstrates how to analyze exported CSV data from the
Bitcoin Dashboard using Python and pandas.

Requirements:
    pip install pandas matplotlib seaborn

Usage:
    1. Export data from dashboard:
       EXPORT_DATA=PRICE > btc_price.csv

    2. Clean the CSV file (remove header/footer lines):
       grep -E "^[0-9]|^timestamp" btc_price.csv > clean_price.csv

    3. Run this script:
       python csv_analysis_example.py clean_price.csv
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import sys
from datetime import datetime

def load_price_data(filename):
    """Load and parse price data CSV."""
    df = pd.read_csv(filename)
    df['timestamp'] = pd.to_datetime(df['timestamp'])
    df.set_index('timestamp', inplace=True)
    return df

def load_block_data(filename):
    """Load and parse block data CSV."""
    df = pd.read_csv(filename)
    df['timestamp'] = pd.to_datetime(df['timestamp'])
    df.set_index('timestamp', inplace=True)
    return df

def load_mempool_data(filename):
    """Load and parse mempool data CSV."""
    df = pd.read_csv(filename)
    df['timestamp'] = pd.to_datetime(df['timestamp'])
    df.set_index('timestamp', inplace=True)
    return df

def analyze_price_data(df):
    """Analyze and visualize price data."""
    print("\n" + "="*60)
    print("PRICE DATA ANALYSIS")
    print("="*60)

    # Basic statistics
    print("\nBTC/USD Statistics:")
    print(f"  Mean:   ${df['price_usd'].mean():.2f}")
    print(f"  Median: ${df['price_usd'].median():.2f}")
    print(f"  Min:    ${df['price_usd'].min():.2f}")
    print(f"  Max:    ${df['price_usd'].max():.2f}")
    print(f"  Std Dev: ${df['price_usd'].std():.2f}")

    print("\nBTC/EUR Statistics:")
    print(f"  Mean:   €{df['price_eur'].mean():.2f}")
    print(f"  Median: €{df['price_eur'].median():.2f}")
    print(f"  Min:    €{df['price_eur'].min():.2f}")
    print(f"  Max:    €{df['price_eur'].max():.2f}")

    # Price change
    start_price = df['price_usd'].iloc[0]
    end_price = df['price_usd'].iloc[-1]
    change = end_price - start_price
    change_pct = (change / start_price) * 100

    print(f"\nPrice Change:")
    print(f"  Start: ${start_price:.2f}")
    print(f"  End:   ${end_price:.2f}")
    print(f"  Change: ${change:+.2f} ({change_pct:+.2f}%)")

    # Volatility (hourly)
    hourly = df.resample('H').last()
    hourly_returns = hourly['price_usd'].pct_change()
    volatility = hourly_returns.std() * 100
    print(f"\nHourly Volatility: {volatility:.2f}%")

    # Plot price over time
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))

    # Price chart
    ax1.plot(df.index, df['price_usd'], label='USD', linewidth=1)
    ax1.plot(df.index, df['price_eur'], label='EUR', linewidth=1, alpha=0.7)
    ax1.set_ylabel('Price')
    ax1.set_title('Bitcoin Price Over Time')
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    # Price change distribution
    price_changes = df['price_usd'].diff()
    ax2.hist(price_changes.dropna(), bins=50, alpha=0.7, edgecolor='black')
    ax2.set_xlabel('Price Change (USD)')
    ax2.set_ylabel('Frequency')
    ax2.set_title('Price Change Distribution')
    ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig('price_analysis.png', dpi=300, bbox_inches='tight')
    print("\nChart saved to: price_analysis.png")

def analyze_block_data(df):
    """Analyze and visualize block data."""
    print("\n" + "="*60)
    print("BLOCK DATA ANALYSIS")
    print("="*60)

    # Basic statistics
    print("\nBlock Statistics:")
    print(f"  Total Blocks: {len(df)}")
    print(f"  Height Range: {df['block_height'].min()} - {df['block_height'].max()}")
    print(f"  Avg TX/Block: {df['tx_count'].mean():.0f}")
    print(f"  Min TX/Block: {df['tx_count'].min()}")
    print(f"  Max TX/Block: {df['tx_count'].max()}")

    # Block time analysis
    if len(df) > 1:
        df['block_interval'] = df.index.to_series().diff().dt.total_seconds() / 60
        avg_interval = df['block_interval'].mean()
        print(f"\nAverage Block Time: {avg_interval:.1f} minutes")
        print(f"Expected: 10.0 minutes")
        print(f"Deviation: {avg_interval - 10:.1f} minutes")

    # Plot block data
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))

    # Transactions per block
    ax1.plot(df.index, df['tx_count'], marker='o', linestyle='-', markersize=3)
    ax1.set_ylabel('Transactions')
    ax1.set_title('Transactions per Block')
    ax1.grid(True, alpha=0.3)
    ax1.axhline(y=df['tx_count'].mean(), color='r', linestyle='--',
                label=f'Average: {df["tx_count"].mean():.0f}')
    ax1.legend()

    # Block intervals
    if 'block_interval' in df.columns:
        ax2.plot(df.index, df['block_interval'], marker='o', linestyle='-',
                markersize=3, color='green')
        ax2.set_ylabel('Minutes')
        ax2.set_title('Block Interval (Time Between Blocks)')
        ax2.grid(True, alpha=0.3)
        ax2.axhline(y=10, color='r', linestyle='--', label='Expected: 10 min')
        ax2.legend()

    plt.tight_layout()
    plt.savefig('block_analysis.png', dpi=300, bbox_inches='tight')
    print("\nChart saved to: block_analysis.png")

def analyze_mempool_data(df):
    """Analyze and visualize mempool data."""
    print("\n" + "="*60)
    print("MEMPOOL DATA ANALYSIS")
    print("="*60)

    # Basic statistics
    print("\nMempool Statistics:")
    print(f"  Avg TX Count: {df['tx_count'].mean():.0f}")
    print(f"  Min TX Count: {df['tx_count'].min()}")
    print(f"  Max TX Count: {df['tx_count'].max()}")
    print(f"  Avg Size: {df['size_mb'].mean():.2f} MB")
    print(f"  Min Size: {df['size_mb'].min():.2f} MB")
    print(f"  Max Size: {df['size_mb'].max():.2f} MB")

    # Correlation
    correlation = df['tx_count'].corr(df['size_mb'])
    print(f"\nCorrelation (TX Count vs Size): {correlation:.3f}")

    # Plot mempool data
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))

    # Transaction count over time
    ax1.plot(df.index, df['tx_count'], linewidth=1)
    ax1.set_ylabel('Pending Transactions')
    ax1.set_title('Mempool Transaction Count')
    ax1.grid(True, alpha=0.3)
    ax1.axhline(y=df['tx_count'].mean(), color='r', linestyle='--',
                label=f'Average: {df["tx_count"].mean():.0f}')
    ax1.legend()

    # Size over time
    ax2.plot(df.index, df['size_mb'], linewidth=1, color='orange')
    ax2.set_ylabel('Size (MB)')
    ax2.set_title('Mempool Size')
    ax2.grid(True, alpha=0.3)
    ax2.axhline(y=df['size_mb'].mean(), color='r', linestyle='--',
                label=f'Average: {df["size_mb"].mean():.2f} MB')
    ax2.legend()

    plt.tight_layout()
    plt.savefig('mempool_analysis.png', dpi=300, bbox_inches='tight')
    print("\nChart saved to: mempool_analysis.png")

def main():
    """Main analysis function."""
    if len(sys.argv) < 2:
        print("Usage: python csv_analysis_example.py <csv_file>")
        print("\nExamples:")
        print("  python csv_analysis_example.py btc_price.csv")
        print("  python csv_analysis_example.py btc_blocks.csv")
        print("  python csv_analysis_example.py btc_mempool.csv")
        sys.exit(1)

    filename = sys.argv[1]

    # Set plotting style
    sns.set_style("whitegrid")
    plt.rcParams['figure.facecolor'] = 'white'

    try:
        # Detect file type from filename
        if 'price' in filename.lower():
            df = load_price_data(filename)
            analyze_price_data(df)
        elif 'block' in filename.lower():
            df = load_block_data(filename)
            analyze_block_data(df)
        elif 'mempool' in filename.lower():
            df = load_mempool_data(filename)
            analyze_mempool_data(df)
        else:
            print(f"Error: Cannot detect data type from filename: {filename}")
            print("Filename should contain 'price', 'block', or 'mempool'")
            sys.exit(1)

        print("\n" + "="*60)
        print("ANALYSIS COMPLETE")
        print("="*60)

    except FileNotFoundError:
        print(f"Error: File not found: {filename}")
        sys.exit(1)
    except Exception as e:
        print(f"Error analyzing data: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
