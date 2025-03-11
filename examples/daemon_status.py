#!/usr/bin/env python
"""
Example to check daemon connectivity and status.

This script demonstrates how to connect to a Monero daemon, verify connection,
and retrieve details such as the daemon's block height and synchronization status.
"""

from monero_cpp import RPCConnection, WalletConfig, Wallet, MoneroError

def main():
    # Setup RPC connection to a Monero daemon.
    daemon = RPCConnection(
        uri="http://localhost:38081",
        username="superuser",
        password="abctesting123"
    )

    # Create a wallet configuration for testing daemon connectivity.
    # This configuration uses a dummy seed for the purpose of the example.
    config = WalletConfig(
        seed="dummy seed for testing daemon connectivity",
        path="DummyWallet",
        password="dummy_password",
        network_type="MAINNET",
        server=daemon,
        restore_height=0
    )

    try:
        # Initialize the wallet.
        wallet = Wallet(config)

        # Check daemon connectivity.
        if wallet.is_connected_to_daemon():
            print("Connected to Monero daemon.")
            print("Daemon connection details:", wallet.get_daemon_connection())
            print("Daemon height:", wallet.get_daemon_height())
            print("Daemon synced:", wallet.is_daemon_synced())
            print("Daemon trusted:", wallet.is_daemon_trusted())
        else:
            print("Not connected to any Monero daemon.")
    except MoneroError as e:
        print("Error during daemon connectivity check:", e)

if __name__ == "__main__":
    main()
