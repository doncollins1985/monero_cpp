"""
Example usage of the monero_cpp module.

This script demonstrates key functionalities including wallet creation,
synchronization, balance queries, and transaction creation.
"""

from monero_cpp import (
    RPCConnection,
    WalletConfig,
    Wallet,
    TxConfig,
    MoneroError
)

def on_sync_progress(height, start_height, end_height, percent_done, message):
    """Callback function to display sync progress."""
    print(f"Sync progress: {percent_done:.1f}% - {message} (Height {height}/{end_height})")

def main():
    # Setup RPC connection to a Monero daemon.
    daemon = RPCConnection(
        uri="http://localhost:38081",
        username="superuser",
        password="abctesting123"
    )

    # Configure wallet restoration using a mnemonic seed.
    config = WalletConfig(
        seed=("hefty value later extra artistic firm radar yodel talent future fungal "
              "nutshell because sanity awesome nail unjustly rage unafraid cedar delayed "
              "thumbs comb custom sanity"),
        path="MyWalletRestored",
        password="supersecretpassword123",
        network_type="STAGENET",  # Options: MAINNET, TESTNET, or STAGENET.
        server=daemon,
        restore_height=380000
    )

    # Create and open the wallet.
    wallet = Wallet(config)
    
    # Add a listener to track synchronization progress.
    wallet.add_listener(on_sync_progress)
    
    # Synchronize the wallet with the blockchain.
    print("Synchronizing wallet...")
    wallet.sync()
    
    # Display wallet and daemon heights.
    print(f"Wallet height: {wallet.get_height()} / Daemon height: {wallet.get_daemon_height()}")
    
    # Display wallet balances.
    balance = wallet.get_balance()
    unlocked_balance = wallet.get_unlocked_balance()
    print(f"Balance: {balance} atomic units")
    print(f"Unlocked Balance: {unlocked_balance} atomic units")
    
    # Create and send a transaction.
    destination = "9tXew6...MoneroAddress...Vz4"  # Replace with a valid Monero address.
    amount = 500000000000  # Amount in atomic units.
    
    try:
        tx_config = TxConfig(destinations=[(destination, amount)], relay=False)
        tx_set = wallet.create_tx(tx_config)
        print(f"Created transaction with hash: {tx_set.tx_hash} and fee: {tx_set.fee}")
        wallet.relay_tx(tx_set)
        print("Transaction successfully relayed!")
    except MoneroError as e:
        print(f"Transaction failed: {e}")

if __name__ == "__main__":
    main()

