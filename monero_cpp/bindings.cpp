#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "monero_tx_config.h"
#include "monero_wallet_config.h"
#include "monero_wallet_full.h"

namespace py = pybind11;

/**
 * @brief Custom exception type for Monero-related errors.
 */
class MoneroError : public std::runtime_error {
public:
  explicit MoneroError(const std::string &msg) : std::runtime_error(msg) {}
};

// Register the custom exception with pybind11.
PYBIND11_MODULE(monero_cpp, m) {
  m.doc() = "Python bindings for monero-cpp library using pybind11";

  // Register the MoneroError exception so that C++ exceptions are translated
  // into Python exceptions.
  py::register_exception<MoneroError>(m, "MoneroError");

  // ---------------------------
  // Bind RPCConnection Class
  // ---------------------------
  py::class_<RPCConnection>(
      m, "RPCConnection",
      "Class representing a connection to a Monero daemon via RPC.")
      .def(
          py::init<const std::string &, const std::string &,
                   const std::string &>(),
          py::arg("uri"), py::arg("username"), py::arg("password"),
          "Initialize a new RPCConnection object.\n\n"
          "Parameters:\n"
          "    uri (str): URI of the daemon (e.g., 'http://localhost:38081').\n"
          "    username (str): Username for authentication.\n"
          "    password (str): Password for authentication.")
      .def_readwrite("uri", &RPCConnection::uri,
                     "The URI of the Monero daemon.")
      .def_readwrite("username", &RPCConnection::username,
                     "Username for the daemon RPC authentication.")
      .def_readwrite("password", &RPCConnection::password,
                     "Password for the daemon RPC authentication.");

  // ---------------------------
  // Bind WalletConfig Class
  // ---------------------------
  py::class_<WalletConfig>(
      m, "WalletConfig",
      "Configuration for creating or restoring a Monero wallet.")
      .def(py::init<const std::string &, const std::string &,
                    const std::string &, const std::string &,
                    const RPCConnection &, uint64_t>(),
           py::arg("seed"), py::arg("path"), py::arg("password"),
           py::arg("network_type"), py::arg("server"),
           py::arg("restore_height") = 0,
           "Create a new WalletConfig.\n\n"
           "Parameters:\n"
           "    seed (str): The mnemonic seed for the wallet.\n"
           "    path (str): Filesystem path to store the wallet file.\n"
           "    password (str): Password to secure the wallet.\n"
           "    network_type (str): Type of network (e.g., 'MAINNET', "
           "'TESTNET', 'STAGENET').\n"
           "    server (RPCConnection): RPC connection settings for the Monero "
           "daemon.\n"
           "    restore_height (int, optional): Block height from which to "
           "start restoring the wallet (default is 0).")
      .def_readwrite("seed", &WalletConfig::seed,
                     "Mnemonic seed for the wallet.")
      .def_readwrite("path", &WalletConfig::path,
                     "File path where the wallet is stored.")
      .def_readwrite("password", &WalletConfig::password,
                     "Password used to secure the wallet.")
      .def_readwrite("network_type", &WalletConfig::network_type,
                     "Network type (e.g., 'MAINNET').")
      .def_readwrite("server", &WalletConfig::server,
                     "RPC connection details for the daemon.")
      .def_readwrite("restore_height", &WalletConfig::restore_height,
                     "Starting block height for wallet restoration.");

  // ---------------------------
  // Bind TxConfig Class
  // ---------------------------
  py::class_<TxConfig>(m, "TxConfig",
                       "Configuration for creating a Monero transaction.")
      .def(py::init<const std::vector<std::pair<std::string, uint64_t>> &,
                    bool>(),
           py::arg("destinations"), py::arg("relay") = false,
           "Initialize a new TxConfig.\n\n"
           "Parameters:\n"
           "    destinations (list of (str, int)): Each tuple contains "
           "(address, amount) where amount is in atomic units.\n"
           "    relay (bool, optional): Whether to relay the transaction "
           "immediately (default is False).")
      .def_readwrite("destinations", &TxConfig::destinations,
                     "List of destination address and amount pairs.")
      .def_readwrite(
          "relay", &TxConfig::relay,
          "Flag indicating whether to relay the transaction immediately.");

  // ---------------------------
  // Bind TransactionSet Class
  // ---------------------------
  py::class_<TransactionSet>(m, "TransactionSet",
                             "A set of transactions created by the wallet.")
      .def("get_serialized", &TransactionSet::get_serialized,
           "Return the serialized signed transaction in hexadecimal format.\n\n"
           "Returns:\n"
           "    str: The hex-encoded signed transaction.")
      .def_property_readonly("tx_hash", &TransactionSet::tx_hash,
                             "Transaction hash of the created transaction.")
      .def_property_readonly("fee", &TransactionSet::fee,
                             "Transaction fee for the created transaction.");

  // ---------------------------
  // Bind Wallet Class
  // ---------------------------
  py::class_<Wallet>(
      m, "Wallet",
      "Monero wallet interface for managing funds and transactions.")
      .def(py::init<const WalletConfig &>(), py::arg("config"),
           "Initialize a new Wallet from the given WalletConfig.\n\n"
           "Parameters:\n"
           "    config (WalletConfig): Configuration object for wallet "
           "creation or restoration.")
      .def("sync", &Wallet::sync,
           "Synchronize the wallet with the blockchain. This method blocks "
           "until synchronization is complete.")
      .def("get_balance", &Wallet::get_balance, py::arg("account_index") = 0,
           "Retrieve the total balance of the wallet (or a specific account if "
           "account_index is provided).\n\n"
           "Parameters:\n"
           "    account_index (int, optional): Account index to query (default "
           "is 0).\n"
           "Returns:\n"
           "    int: The balance in atomic units.")
      .def("get_unlocked_balance", &Wallet::get_unlocked_balance,
           py::arg("account_index") = 0,
           "Retrieve the unlocked balance of the wallet (or a specific account "
           "if account_index is provided).\n\n"
           "Parameters:\n"
           "    account_index (int, optional): Account index to query (default "
           "is 0).\n"
           "Returns:\n"
           "    int: The unlocked balance in atomic units.")
      .def("get_primary_address", &Wallet::get_primary_address,
           "Return the primary address of the wallet.\n\n"
           "Returns:\n"
           "    str: The primary Monero address associated with the wallet.")
      .def("get_height", &Wallet::get_height,
           "Return the current block height of the wallet.\n\n"
           "Returns:\n"
           "    int: The block height to which the wallet is synchronized.")
      .def("get_daemon_height", &Wallet::get_daemon_height,
           "Return the current block height of the connected daemon.\n\n"
           "Returns:\n"
           "    int: The daemon's current block height.")
      .def("is_connected_to_daemon", &Wallet::is_connected_to_daemon,
           "Check if the wallet is connected to a Monero daemon.\n\n"
           "Returns:\n"
           "    bool: True if connected, False otherwise.")
      .def("get_daemon_connection", &Wallet::get_daemon_connection,
           "Return the current RPC connection details of the daemon.\n\n"
           "Returns:\n"
           "    dict: A dictionary containing connection details (e.g., 'uri', "
           "'username').")
      .def("is_daemon_synced", &Wallet::is_daemon_synced,
           "Check if the connected daemon is fully synchronized.\n\n"
           "Returns:\n"
           "    bool: True if synchronized, False otherwise.")
      .def("is_daemon_trusted", &Wallet::is_daemon_trusted,
           "Check if the connected daemon is trusted.\n\n"
           "Returns:\n"
           "    bool: True if trusted, False otherwise.")
      .def("create_tx", &Wallet::create_tx, py::arg("tx_config"),
           "Create a transaction using the provided TxConfig.\n\n"
           "Parameters:\n"
           "    tx_config (TxConfig): Configuration for the transaction.\n"
           "Returns:\n"
           "    TransactionSet: An object representing the created "
           "transaction(s).")
      .def("relay_tx", &Wallet::relay_tx, py::arg("tx_set"),
           "Relay a previously created transaction to the network.\n\n"
           "Parameters:\n"
           "    tx_set (TransactionSet): The transaction set to be relayed.")
      .def("add_listener", &Wallet::add_listener, py::arg("callback"),
           "Register a callback function to receive wallet events (e.g., sync "
           "progress, new transfers).\n\n"
           "Parameters:\n"
           "    callback (callable): A Python function that will be called "
           "with event data.");
}
