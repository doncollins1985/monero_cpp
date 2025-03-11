## Usage Examples

To help developers get started, the module will include clear Python examples demonstrating key functionalities such as wallet creation, balance queries, transfers, and daemon interaction ([Monero Observer - everoddandeven proposes bounty for monero-cpp Python bindings module](https://monero.observer/everoddandeven-bounty-monero-cpp-library-python-bindings-module/#:~:text=,to%20install%20via%20pip)). These examples will serve as both documentation and tests of typical usage. Below are a few representative scenarios with example code:

- **Creating and Syncing a Wallet**: The following code snippet shows how to create a new wallet from a seed phrase and sync it with a Monero daemon: 

```python
from monero_cpp import Wallet, WalletConfig, RPCConnection

# Configure connection to a Monero daemon (remote node or local monerod)
daemon = RPCConnection(uri="http://localhost:38081", username="superuser", password="abctesting123")

# Set up wallet configuration for restoring from a seed
config = WalletConfig(
    seed="hefty value later extra artistic firm radar yodel talent future fungal nutshell because sanity awesome nail unjustly rage unafraid cedar delayed thumbs comb custom sanity",  # 25-word Monero seed
    path="MyWalletRestored",
    password="supersecretpassword123",
    network_type="STAGENET",      # or "MAINNET"/"TESTNET"
    server=daemon,
    restore_height=380000        # start sync from a specific block height
)

# Create and restore the wallet
wallet = Wallet(config)  # this initializes a monero-cpp full wallet under the hood

# Synchronize the wallet with the blockchain, showing progress
def on_sync_progress(height, start_height, end_height, percent_done, message):
    print(f"Sync progress: {percent_done:.1f}% - {message}")

wallet.add_listener(on_sync_progress)       # register a Python callback for sync updates
wallet.sync()                               # perform sync (blocking until completion)
print(f"Wallet synchronized at block: {wallet.get_height()} of {wallet.get_daemon_height()}")
```

In this example, we created a `Wallet` using a seed. The `WalletConfig` class makes it easy to specify all needed parameters in a Pythonic way (seed, path, password, network, node, etc.). We then attached a listener to print sync progress (the binding supports Python callables as listeners for the `on_sync_progress` event). Finally, we called `wallet.sync()` to fetch the blockchain data; once done, we print the wallet’s height versus daemon height to confirm it’s fully synced. All these operations mirror the monero-cpp capabilities (create wallet, set daemon connection, sync, get heights) but through an intuitive Python API.

- **Checking Balances and Addresses**: After syncing, we can query the wallet’s balances and addresses:

```python
# Get primary address and balance
primary_address = wallet.get_primary_address()
balance = wallet.get_balance()           # total balance (across accounts)
unlocked = wallet.get_unlocked_balance() # total unlocked balance

print(f"Primary address: {primary_address}")
print(f"Balance: {balance} atomic units, Unlocked: {unlocked} atomic units")

# You can also get balance per account or subaddress
acct_1_balance = wallet.get_balance(account_index=1)
print(f"Account 1 balance: {acct_1_balance}")
```

This demonstrates simple getters. The addresses and balances are returned as Python strings and integers (Monero balances are in the smallest unit, often called “piconero” – the binding will just use integers for atomic units, leaving formatting to the user).

- **Transaction Creation and Sending**: Sending Monero is straightforward. For example:

```python
from monero_cpp import TxConfig, MoneroError

destination = "9tXew6...MoneroAddress...Vz4"  # some Monero address
amount = 5_000_000_000000000  # 5 XMR in atomic units (example amount)

try:
    tx_config = TxConfig(destinations=[{"address": destination, "amount": amount}], relay=False)
    tx_set = wallet.create_tx(tx_config)    # create transaction (does not relay yet)
    signed_tx_hex = tx_set.get_serialized() # get signed transaction in hex form
    print(f"Created transaction {tx_set.tx_hash} with fee {tx_set.fee}")
    
    wallet.relay_tx(tx_set)                 # relay the transaction to the network
    print("Transaction successfully sent!")
except MoneroError as e:
    print(f"Failed to send transaction: {e}")
```

In this snippet, we first construct a `TxConfig` object to specify the transaction parameters. We use one destination (address and amount). The `relay=False` indicates we want to create the transaction without immediately relaying it (perhaps to review or export it first; if we wanted an automatic send, we could set relay True or call a convenience method). We then call `wallet.create_tx(tx_config)`, which returns a transaction set/object (wrapping one or multiple transactions – monero-cpp uses `monero_tx_set` and `monero_tx_wallet` to represent the created transaction and its metadata). We retrieve the transaction hash and fee from it and then explicitly call `wallet.relay_tx(tx_set)` to submit it to the network. The code is wrapped in a try/except to catch any `MoneroError` (for example, if the daemon is not connected, or the amount is too large, an exception will be thrown with an appropriate message). 

The example shows how easy it is to send funds: fill a config, call `create_tx`, then `relay_tx`. The binding handles converting the Python `TxConfig` into the C++ monero-cpp structures and back. It also demonstrates error handling – the Python exception `MoneroError` carries the message from the underlying C++ error (if any).

- **Daemon Interaction**: Some information can be fetched from the connected daemon via the wallet. For instance:

```python
if wallet.is_connected_to_daemon():
    info = wallet.get_daemon_connection()
    height = wallet.get_daemon_height()
    print(f"Connected to daemon at {info['uri']}, daemon height = {height}")
    print(f"Daemon synchronized: {wallet.is_daemon_synced()}, trusted: {wallet.is_daemon_trusted()}")
```

This uses methods that wrap monero-cpp’s daemon queries (e.g., `get_daemon_height()` and status checks). In this case, `wallet.get_daemon_connection()` might return an object or dict with the URI, username, etc. (the binding can choose to present it as a Python `dict` for convenience). We then get the current block height of the daemon and whether the wallet believes the daemon is synced and trusted. Such calls are useful for feedback in applications to ensure the backend is ready.

- **Additional Examples**: We will also include examples of more advanced usage, such as creating a **multisig wallet** or exchanging multisig synchronization data between wallets, using the address book (adding and retrieving saved addresses), or sweeping all funds from an account. These illustrate that even complex workflows can be done in Python by orchestrating calls to the binding. For instance, a multisig example might show creating N-of-M multisig wallets and then demonstrate signing a transaction by multiple parties.

All example code will be provided in the repository (in an `examples/` folder or as part of documentation). This helps new users quickly learn how to use the module for common tasks. It also serves as a form of integration test – ensuring that the high-level scenarios actually work with the binding.

Each example will be clearly commented and possibly referenced in the docstrings (for example, the Wallet class docstring might say “See `examples/create_wallet.py` for an example of restoring a wallet from seed.”). By following these examples, developers can confidently integrate the monero-cpp binding into their own projects, knowing they are using the API correctly and efficiently.
