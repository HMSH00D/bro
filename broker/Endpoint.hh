#ifndef BROKER_ENDPOINT_HH
#define BROKER_ENDPOINT_HH

#include <broker/Peer.hh>

#include <memory>
#include <string>
#include <cstdint>
#include <chrono>

namespace broker {

class PrintHandler;

class Endpoint {
friend class broker::PrintHandler;

public:

	/**
	 * Create a local broker endpoint.
	 * @param name a descriptive name for this endpoint.
	 * @param flags tune the behavior of the endpoint.  No flags exist yet.
	 */
	Endpoint(std::string name, int flags = 0);

	/**
	 * Shutdown the local broker endpoint and disconnect from peers.
	 */
	~Endpoint();

	/**
	 * @return the descriptive name for this endpoint (as given to ctor).
	 */
	const std::string& Name() const;

	/**
	 * @return an error code associated with the last failed endpoint operation.
	 * If non-zero, it may be passed to broker::strerror() for a description.
	 */
	int LastErrno() const;

	/**
	 * @return descriptive error text associated with the last failed enpoint
	 * operation.
	 */
	const std::string& LastError() const;

	/**
	 * Make this local broker endpoint available for remote peer connections.
	 * @param port the TCP port on which to accept connections.
	 * @param addr an address to accept on, e.g. "127.0.0.1".
	 *             A nullptr refers to @p INADDR_ANY.
	 * @return true if the endpoint is now listening, else false.  For the
	 *         later case, LastError() contains descriptive error text and
	 *         LastErrno(), if non-zero, is an error code set by @p bind(2).
	 */
	bool Listen(uint16_t port, const char* addr = nullptr);

	/**
	 * Connect to a remote endpoint.
	 * @param addr an address to connect to, e.g. "localhost" or "127.0.0.1".
	 * @param port the TCP port on which the remote is listening.
	 * @param retry an interval at which to retry establishing the connection
	 *        with the remote peer.
	 * @return a peer object that this endpoint can use to identify the
	 *         particular peer established by this method.
	 */
	Peer AddPeer(std::string addr, uint16_t port,
	             std::chrono::duration<double> retry = std::chrono::seconds(5));

	/**
	 * Connect to a local endpoint.
	 * @param e another local endpoint.
	 * @return a peer object that this endpoint can use to identify the
	 *         particular peer established by this method.
	 */
	Peer AddPeer(const Endpoint& e);

	/**
	 * Remove a connection to a peer endpoint.
	 * @param peerno a peer object previously returned by AddPeer().
	 * @return false if no such peer associated exists, else true (and the
	 *         peering is no more).
	 */
	bool RemPeer(Peer peer);

	/**
	 * Sends a message string to all PrintHandler's for a given topic that are
	 * connected to this endpoint directly or indirectly through peer endpoints.
	 * @param topic the topic associated with the message.
	 * @param msg a string to send all handlers for the topic.
	 */
	void Print(std::string topic, std::string msg) const;

	// TODO: Event() is similar to Print but w/ fancier arguments.
	// TODO: Log() is similar to Print but w/ fancier arguments.

private:

	class Impl;
	std::unique_ptr<Impl> p;
};

} // namespace broker

#endif // BROKER_ENDPOINT_HH